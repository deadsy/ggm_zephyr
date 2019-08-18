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
 * jack functions
 */

struct jack {
	struct synth *synth;
	jack_client_t *client;
};

static int jack_process(jack_nframes_t nframes, void *arg)
{
	LOG_INF("");
	return 0;
}

static void jack_shutdown(void *arg)
{
	LOG_INF("");
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
	err = jack_set_process_callback(j->client, jack_process, NULL);
	if (err != 0) {
		LOG_ERR("jack_set_process_callback() error %d", err);
		goto error;
	}

	/* tell the JACK server to call shutdown() if it ever shuts down,
	 * either entirely, or if it just decides to stop calling us.
	 */
	jack_on_shutdown(j->client, jack_shutdown, NULL);

	/* audio input ports */
	if (s->audio_in > 0) {
	}

	// output_port = jack_port_register (j->client, "audio_in", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

	/* audio output ports */
	if (s->audio_out > 0) {
	}


	/* MIDI input ports */
	if (s->midi_in > 0) {
	}

	/* MIDI output ports */
	if (s->midi_out > 0) {
	}


	return j;

error:
	if (j->client != NULL) {
		jack_client_close(j->client);
		j->client = NULL;
	}
	ggm_free(j);
	return NULL;
}

static void jack_del(struct jack *j)
{
	LOG_INF("");
	jack_client_close(j->client);
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
