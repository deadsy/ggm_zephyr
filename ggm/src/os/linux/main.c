/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define GGM_MAIN

#include <jack/jack.h>

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
 * jack ports
 */

static void jack_free_ports(
	jack_client_t *client,
	size_t n,
	const char *base_name,
	jack_port_t **port
	)
{
	if (port == NULL) {
		return;
	}

	for (size_t i = 0; i < n; i++) {
		if (port[i] != NULL) {
			char name[128];
			snprintf(name, sizeof(name), "%s%lu", base_name, i);
			int err = jack_port_unregister(client, port[i]);
			if (err != 0) {
				LOG_ERR("unable to unregister %s", name);
			}
			LOG_DBG("unregistered %s", name);
		}
	}

	ggm_free(port);
}


static jack_port_t **jack_alloc_ports(
	jack_client_t *client,
	size_t n,
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
		snprintf(name, sizeof(name), "%s%lu", base_name, i);
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
	LOG_INF("");
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
		jack_free_ports(j->client, s->audio_in, "audio_in", j->audio_in);
		jack_free_ports(j->client, s->audio_out, "audio_out", j->audio_out);
		jack_free_ports(j->client, s->midi_in, "midi_in", j->midi_in);
		jack_free_ports(j->client, s->midi_out, "midi_out", j->midi_out);
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
	if (s->audio_in > 0) {
		j->audio_in = jack_alloc_ports(j->client, s->audio_in, "audio_in", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput);
		if (j->audio_in == NULL) {
			goto error;
		}
	}

	/* audio output ports */
	if (s->audio_out > 0) {
		j->audio_out = jack_alloc_ports(j->client, s->audio_out, "audio_out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput);
		if (j->audio_out == NULL) {
			goto error;
		}
	}

	/* MIDI input ports */
	if (s->midi_in > 0) {
		j->midi_in = jack_alloc_ports(j->client, s->midi_in, "midi_in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput);
		if (j->midi_in == NULL) {
			goto error;
		}
	}

	/* MIDI output ports */
	if (s->midi_out > 0) {
		j->midi_out = jack_alloc_ports(j->client, s->midi_out, "midi_out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput);
		if (j->midi_out == NULL) {
			goto error;
		}
	}

	return j;

error:

	jack_free_ports(j->client, s->audio_in, "audio_in", j->audio_in);
	jack_free_ports(j->client, s->audio_out, "audio_out", j->audio_out);
	jack_free_ports(j->client, s->midi_in, "midi_in", j->midi_in);
	jack_free_ports(j->client, s->midi_out, "midi_out", j->midi_out);

	if (j->client != NULL) {
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

	// for (int i = 0; i < 3000; i++) {
	//	synth_loop(s);
	//	ggm_mdelay(3);
	// }

exit:
	jack_del(j);
	synth_del(s);
	return 0;
}

/*****************************************************************************/
