/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ggm.h"

/******************************************************************************
 * event queue - events are dispatched to modules between calls to process
 * audio buffers. Events generated during buffer processing are queued and then
 * dispatched after the buffer processing is completed.
 */

/* synth_event_rd reads an event from the event queue */
static int synth_event_rd(struct synth *s, struct qevent *e)
{
	struct event_queue *eq = &s->eq;
	int rc = 0;

	/* do we have events? */
	if (eq->rd == eq->wr) {
		/* no events */
		rc = -1;
		goto exit;
	}

	/* copy the event data */
	struct qevent *x = &eq->queue[eq->rd];
	memcpy(e, x, sizeof(struct qevent));

	/* advance the read index */
	eq->rd = (eq->rd + 1) & (NUM_EVENTS - 1);

exit:
	return rc;
}

/* synth_event_wr writes an event to the event queue */
int synth_event_wr(struct synth *s, struct module *m, int idx, const struct event *e)
{
	struct event_queue *eq = &s->eq;
	int rc = 0;

	/* do we have queue space? */
	size_t wr = (eq->wr + 1) & (NUM_EVENTS - 1);

	if (wr == eq->rd) {
		/* the queue is full */
		rc = -1;
		goto exit;
	}

	/* copy the event data */
	struct qevent *x = &eq->queue[eq->wr];
	x->m = m;
	x->idx = idx;
	memcpy(&x->e, e, sizeof(struct event));

	/* advance the write index */
	eq->wr = wr;

exit:
	return rc;
}

/******************************************************************************
 * synth_new allocates a new synth.
 */

struct synth *synth_new(void)
{
	/* allocate the synth */
	struct synth *s = ggm_calloc(1, sizeof(struct synth));

	if (s == NULL) {
		LOG_ERR("could not allocate synth");
		return NULL;
	}

	return s;
}

/******************************************************************************
 * synth_del closes a synth and deallocates resources.
 */

void synth_del(struct synth *s)
{
	if (s == NULL) {
		return;
	}

	module_del(s->root);

	/* free the allocated audio buffers */
	ggm_free(s->bufs[0]);

	ggm_free(s);
}

/******************************************************************************
 * synth_midi_out is called when the running top-level module has a MIDI
 * message to output. It has the prototype of a port function, and the module
 * will be the root module.
 */

static void synth_midi_out(struct module *m, const struct event *e)
{
	LOG_INF("TODO");
}

/******************************************************************************
 * synth_set_root sets the root patch of the synth.
 */

int synth_set_root(struct synth *s, struct module *m)
{
	LOG_INF("%s", m->name);

	/* how many audio buffers do we need? */
	size_t nbufs = port_count_by_type(m->info->in, PORT_TYPE_AUDIO);
	nbufs += port_count_by_type(m->info->out, PORT_TYPE_AUDIO);
	if (nbufs > NUM_AUDIO_PORTS) {
		LOG_ERR("number of audio input/output ports > NUM_AUDIO_PORTS");
		return -1;
	}

	/* allocate the audio buffers */
	float *buf = ggm_calloc(nbufs, AudioBufferSize * sizeof(float));
	if (buf == NULL) {
		LOG_ERR("could not allocate audio buffers");
		return -1;
	}

	/* setup the audio buffer list */
	for (size_t i = 0; i < nbufs; i++) {
		s->bufs[i] = &buf[i * AudioBufferSize];
	}

	/* hookup up any MIDI output to the top-level callback */
	/* TODO fix for n outputs */
	int idx = port_get_index(m->info->out, "midi");
	if (idx >= 0) {
		port_add_dst(m, idx, m, synth_midi_out);
	}

	s->root = m;
	return 0;
}

bool synth_has_root(struct synth *s)
{
	return s->root != NULL;
}

/******************************************************************************
 * synth_loop runs the top-level synth loop
 */

void synth_loop(struct synth *s)
{
	struct module *m = s->root;
	struct qevent q;

	/* run the buffer processing */
	m->info->process(m, s->bufs);

	/* process all queued events */
	while (synth_event_rd(s, &q) == 0) {
		event_out(q.m, q.idx, &q.e);
	}
}

/*****************************************************************************/
