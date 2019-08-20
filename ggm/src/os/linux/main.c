/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define GGM_MAIN

#include <jack/jack.h>
#include <jack/midiport.h>

#include "ggm.h"
#include "module.h"

/******************************************************************************
 * jack data
 */

struct jack {
	struct synth *synth;
	jack_client_t *client;
	jack_port_t **audio_in;         /* audio input ports */
	jack_port_t **audio_out;        /* audio output ports */
	jack_port_t **midi_in;          /* MIDI input ports */
	jack_port_t **midi_out;         /* MIDI output ports */
};

/******************************************************************************
 * convert jack MIDI buffer to MIDI event
 */

static int jack_convert_midi_event(struct event *dst, jack_midi_event_t *src)
{
	uint8_t *buf = src->buffer;
	unsigned int n = src->size;

	LOG_INF("time %d size %d buffer %p", src->time, src->size, src->buffer);

	if (n == 0) {
		LOG_WRN("jack midi event has no data");
		return -1;
	}

	uint8_t status = buf[0];

	if (status < MIDI_STATUS_COMMON) {
		/* channel message */
		switch (status & 0xf0) {
		case MIDI_STATUS_NOTEOFF:
		case MIDI_STATUS_NOTEON:
		case MIDI_STATUS_POLYPHONICAFTERTOUCH:
		case MIDI_STATUS_CONTROLCHANGE:
		case MIDI_STATUS_PITCHWHEEL: {
			if (n == 3) {
				event_set_midi(dst, buf[0], buf[1], buf[2]);
				return 0;
			}
			LOG_WRN("jack midi event size != 3");
			break;
		}
		case MIDI_STATUS_PROGRAMCHANGE:
		case MIDI_STATUS_CHANNELAFTERTOUCH: {
			if (n == 2) {
				event_set_midi(dst, buf[0], buf[1], 0);
				return 0;
			}
			LOG_WRN("jack midi event size != 2");
			break;
		}
		default:
			LOG_WRN("unhandled channel msg %02x", status);
			break;
		}
	} else if (status < MIDI_STATUS_REALTIME) {
		/* system common message */
		switch (status) {
		case MIDI_STATUS_SYSEXSTART:
		case MIDI_STATUS_QUARTERFRAME:
		case MIDI_STATUS_SONGPOINTER:
		case MIDI_STATUS_SONGSELECT:
		case MIDI_STATUS_TUNEREQUEST:
		case MIDI_STATUS_SYSEXEND:
		default:
			LOG_WRN("unhandled system commmon msg %02x", status);
			break;
		}
	} else {
		/* system real time message */
		switch (status) {
		case MIDI_STATUS_TIMINGCLOCK:
		case MIDI_STATUS_START:
		case MIDI_STATUS_CONTINUE:
		case MIDI_STATUS_STOP:
		case MIDI_STATUS_ACTIVESENSING:
		case MIDI_STATUS_RESET:
		default:
			LOG_WRN("unhandled system realtime msg %02x", status);
			break;
		}
	}

	return -1;
}

/******************************************************************************
 * jack ports
 */

static void jack_free_ports(
	jack_client_t *client,
	unsigned int n,
	const char *base_name,
	jack_port_t **port
	)
{
	if (port == NULL) {
		return;
	}

	for (size_t i = 0; i < n; i++) {
		if (port[i] != NULL) {
			int err = jack_port_unregister(client, port[i]);
			if (err != 0) {
				LOG_ERR("unable to unregister %s_%lu", base_name, i);
			}
			LOG_DBG("unregistered %s_%lu", base_name, i);
		}
	}

	ggm_free(port);
}


static jack_port_t **jack_alloc_ports(
	jack_client_t *client,
	unsigned int n,
	const char *base_name,
	const char *type,
	unsigned long flags
	)
{
	if (n == 0) {
		return NULL;
	}

	jack_port_t **port = (jack_port_t **)ggm_calloc(n, sizeof(jack_port_t *));
	if (port == NULL) {
		LOG_ERR("unable to allocate port array");
		return NULL;
	}

	for (size_t i = 0; i < n; i++) {
		char name[128];
		snprintf(name, sizeof(name), "%s_%lu", base_name, i);
		port[i] = jack_port_register(client, name, type, flags, 0);
		if (port[i] == NULL) {
			LOG_ERR("unable to register port %s", name);
			goto error;
		}
		LOG_DBG("registered %s", name);
	}

	return port;

error:
	jack_free_ports(client, n, base_name, port);
	return NULL;
}

/******************************************************************************
 * jack callbacks
 */

static int jack_process(jack_nframes_t nframes, void *arg)
{
	struct jack *j = (struct jack *)arg;
	struct synth *s = (struct synth *)j->synth;
	unsigned int i;

	// LOG_DBG("nframes %d", nframes);

	/* read MIDI input events */
	for (i = 0; i < s->n_midi_in; i++) {
		void *buf = jack_port_get_buffer(j->midi_in[i], nframes);
		if (buf == NULL) {
			LOG_ERR("jack_port_get_buffer() returned NULL");
			continue;
		}
		jack_nframes_t n = jack_midi_get_event_count(buf);
		for (unsigned int idx = 0; idx < n; idx++) {
			jack_midi_event_t event;
			int err = jack_midi_event_get(&event, buf, idx);
			if (err != 0) {
				LOG_ERR("jack_midi_event_get() returned %d", err);
				continue;
			}
			/* forward the MIDI event to the root module of the synth  */
			struct event e;
			jack_convert_midi_event(&e, &event);
			synth_midi_in(s, i, &e);
		}
	}

	/* read from the audio input buffers */
	for (i = 0; i < s->n_audio_in; i++) {
		float *buf = (float *)jack_port_get_buffer(j->audio_in[i], nframes);
		block_copy(s->bufs[i], buf);
	}

	/* run the synth loop */
	synth_loop(s);

	/* write to the audio output buffers */
	unsigned int ofs = s->n_audio_in;
	for (i = 0; i < s->n_audio_out; i++) {
		float *buf = (float *)jack_port_get_buffer(j->audio_out[i], nframes);
		block_copy(buf, s->bufs[ofs + i]);
	}

	/* write MIDI output events */
	/* TODO */

	return 0;
}

static void jack_shutdown(void *arg)
{
	LOG_INF("");
}

/******************************************************************************
 * jack new/del
 */

static void jack_del(struct jack *j)
{
	LOG_INF("");
	if (j == NULL) {
		return;
	}
	if (j->client != NULL) {
		struct synth *s = j->synth;
		jack_deactivate(j->client);
		jack_free_ports(j->client, s->n_audio_in, "audio_in", j->audio_in);
		jack_free_ports(j->client, s->n_audio_out, "audio_out", j->audio_out);
		jack_free_ports(j->client, s->n_midi_in, "midi_in", j->midi_in);
		jack_free_ports(j->client, s->n_midi_out, "midi_out", j->midi_out);
		jack_client_close(j->client);
	}
	ggm_free(j);
}

static struct jack *jack_new(struct synth *s)
{
	jack_status_t status;
	int err;

	LOG_INF("jack version %s", jack_get_version_string());

	if (!synth_has_root(s)) {
		LOG_ERR("synth does not have a root patch set");
		return NULL;
	}

	struct jack *j = ggm_calloc(1, sizeof(struct jack));
	if (j == NULL) {
		LOG_ERR("cannot allocate jack data");
		return NULL;
	}

	/* setup the jack data */
	j->synth = s;

	/* open the client */
	j->client = jack_client_open("ggm", JackNoStartServer, &status);
	if (j->client == NULL) {
		LOG_ERR("jack server not running");
		goto error;
	}

	/* check sample rate */
	jack_nframes_t rate = jack_get_sample_rate(j->client);
	if (rate != AudioSampleFrequency) {
		LOG_ERR("jack sample rate %d != ggm sample rate %d", rate, AudioSampleFrequency);
		goto error;
	}

	/* check audio buffer size */
	jack_nframes_t bufsize = jack_get_buffer_size(j->client);
	if (bufsize != AudioBufferSize) {
		LOG_ERR("jack buffer size %d != ggm buffer size %d", bufsize, AudioBufferSize);
		goto error;
	}

	/* tell the JACK server to call jack_process() whenever there is work to be done. */
	err = jack_set_process_callback(j->client, jack_process, (void *)j);
	if (err != 0) {
		LOG_ERR("jack_set_process_callback() error %d", err);
		goto error;
	}

	/* tell the JACK server to call shutdown() if it ever shuts down,
	 * either entirely, or if it just decides to stop calling us.
	 */
	jack_on_shutdown(j->client, jack_shutdown, (void *)j);

	/* audio input ports */
	if (s->n_audio_in > 0) {
		j->audio_in = jack_alloc_ports(j->client, s->n_audio_in, "audio_in", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput);
		if (j->audio_in == NULL) {
			goto error;
		}
	}

	/* audio output ports */
	if (s->n_audio_out > 0) {
		j->audio_out = jack_alloc_ports(j->client, s->n_audio_out, "audio_out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput);
		if (j->audio_out == NULL) {
			goto error;
		}
	}

	/* MIDI input ports */
	if (s->n_midi_in > 0) {
		j->midi_in = jack_alloc_ports(j->client, s->n_midi_in, "midi_in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput);
		if (j->midi_in == NULL) {
			goto error;
		}
	}

	/* MIDI output ports */
	if (s->n_midi_out > 0) {
		j->midi_out = jack_alloc_ports(j->client, s->n_midi_out, "midi_out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput);
		if (j->midi_out == NULL) {
			goto error;
		}
	}

	/* Tell the JACK server we are ready to roll.
	 * Our process() callback will start running now.
	 */
	err = jack_activate(j->client);
	if (err != 0) {
		LOG_ERR("jack_activate() error %d", err);
		goto error;
	}

	return j;

error:

	if (j->client != NULL) {
		jack_free_ports(j->client, s->n_audio_in, "audio_in", j->audio_in);
		jack_free_ports(j->client, s->n_audio_out, "audio_out", j->audio_out);
		jack_free_ports(j->client, s->n_midi_in, "midi_in", j->midi_in);
		jack_free_ports(j->client, s->n_midi_out, "midi_out", j->midi_out);
		jack_client_close(j->client);
	}

	ggm_free(j);

	return NULL;
}

/******************************************************************************
 * main
 */

int main(void)
{
	struct jack *j = NULL;
	int err;

	log_set_prefix("ggm/src/");

	LOG_INF("GooGooMuck %s (%s)", GGM_VERSION, CONFIG_BOARD);

	struct synth *s = synth_new();
	if (s == NULL) {
		goto exit;
	}

	struct module *m = module_new(s, "root.poly");
	if (m == NULL) {
		goto exit;
	}

	err = synth_set_root(s, m);
	if (err != 0) {
		goto exit;
	}

	j = jack_new(s);
	if (j == NULL) {
		goto exit;
	}

	while (1) {
		ggm_mdelay(100);
	}

exit:
	jack_del(j);
	synth_del(s);
	return 0;
}

/*****************************************************************************/
