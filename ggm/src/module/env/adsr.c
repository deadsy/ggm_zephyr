/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Attack/Decay/Sustain/Release Envelope Generator
 */

#include "ggm.h"

/******************************************************************************
 * private state
 */

enum adsr_state {
	ADSR_STATE_IDLE = 0, /* initial state */
	ADSR_STATE_ATTACK,
	ADSR_STATE_DECAY,
	ADSR_STATE_SUSTAIN,
	ADSR_STATE_RELEASE,
	ADSR_STATE_RESET,
};

struct adsr {
	enum adsr_state state;          /* envelope state */
	float s;                        /* sustain level */
	float ka;                       /* attack constant */
	float kd;                       /* decay constant */
	float kr;                       /* release constant */
	float k_reset;                  /* soft reset constant */
	float d_trigger;                /* attack->decay trigger level */
	float s_trigger;                /* decay->sustain trigger level */
	float i_trigger;                /* release->idle trigger level */
	float val;                      /* output value */
};

/* When we need to shutdown a voice we do it slowly to avoid any clicks in
 * the output. The soft reset of the ADSR envelope does this. It's essentially
 * the same as the release state, but works over a constant/short time period.
 */
#define SOFT_RESET_TIME 30e-3f

/******************************************************************************
 * We can't reach the target level with the asymptotic rise/fall of exponentials.
 * We will change state when we are within LEVEL_EPSILON of the target level.
 */

#define LEVEL_EPSILON (0.001f)
#define LN_LEVEL_EPSILON (-6.9077553f)  /* ln(LEVEL_EPSILON) */

/* Return a k value to give the exponential rise/fall in the required time. */
static float get_k(float t, int rate)
{
	if (t <= 0.f) {
		return 1.f;
	}
	return 1.f - powe(LN_LEVEL_EPSILON / (t * (float)rate));
}

/******************************************************************************
 * module port functions
 */

/* adsr_port_reset resets the state of the envelope */
static void adsr_port_reset(struct module *m, const struct event *e)
{
	struct adsr *this = (struct adsr *)m->priv;
	bool reset = event_get_bool(e);

	if (reset) {
		LOG_DBG("%s_%08x hard reset", m->info->name, m->id);
		if (this->state != ADSR_STATE_IDLE) {
			/* This is likely to cause clicks in the output.
			 * A soft reset prior to this time would be nicer.
			 */
			LOG_WRN("forced idle");
		}
		this->val = 0.f;
		this->state = ADSR_STATE_IDLE;
	} else {
		LOG_DBG("%s_%08x soft reset", m->info->name, m->id);
		if (this->state != ADSR_STATE_IDLE) {
			this->state = ADSR_STATE_RESET;
		}
	}
}

/* adsr_port_gate is the envelope gate control, attack(>0) or release(=0) */
static void adsr_port_gate(struct module *m, const struct event *e)
{
	struct adsr *this = (struct adsr *)m->priv;
	float gate = event_get_float(e);

	LOG_DBG("%s_%08x gate %f", m->info->name, m->id, gate);

	/* attack */
	if (gate > 0.f) {
		this->state = ADSR_STATE_ATTACK;
		return;
	}

	/* release */
	if (this->state != ADSR_STATE_IDLE) {
		if (this->kr == 1.f) {
			/* no release - goto idle */
			this->val = 0.f;
			this->state = ADSR_STATE_IDLE;
		} else {
			this->state = ADSR_STATE_RELEASE;
		}
	}
}

/* adsr_port_attack sets the attack time (secs) */
static void adsr_port_attack(struct module *m, const struct event *e)
{
	struct adsr *this = (struct adsr *)m->priv;
	float attack = clampf_lo(event_get_float(e), 0.f);

	LOG_DBG("%s_%08x set attack time %f secs", m->info->name, m->id, attack);
	this->ka = get_k(attack, AudioSampleFrequency);
}

/* adsr_port_decay sets the decay time (secs) */
static void adsr_port_decay(struct module *m, const struct event *e)
{
	struct adsr *this = (struct adsr *)m->priv;
	float decay = clampf_lo(event_get_float(e), 0.f);

	LOG_DBG("%s_%08x set decay time %f secs", m->info->name, m->id, decay);
	this->kd = get_k(decay, AudioSampleFrequency);
}

/* adsr_port_sustain sets the sustain level 0..1 */
static void adsr_port_sustain(struct module *m, const struct event *e)
{
	struct adsr *this = (struct adsr *)m->priv;
	float sustain = clampf(event_get_float(e), 0.f, 1.f);

	LOG_DBG("%s_%08x set sustain level %f", m->info->name, m->id, sustain);
	this->s = sustain;
	this->d_trigger = 1.f - LEVEL_EPSILON;
	this->s_trigger = sustain + (1.f - sustain) * LEVEL_EPSILON;
	this->i_trigger = sustain * LEVEL_EPSILON;
}

/* adsr_port_release sets the release time (secs) */
static void adsr_port_release(struct module *m, const struct event *e)
{
	struct adsr *this = (struct adsr *)m->priv;
	float release = clampf_lo(event_get_float(e), 0.f);

	LOG_DBG("%s_%08x set release time %f secs", m->info->name, m->id, release);
	this->kr = get_k(release, AudioSampleFrequency);
}

/******************************************************************************
 * module functions
 */

static int adsr_alloc(struct module *m, va_list vargs)
{
	/* allocate the private data */
	struct adsr *this = ggm_calloc(1, sizeof(struct adsr));

	if (this == NULL) {
		return -1;
	}
	m->priv = (void *)this;

	/* set the soft reset time */
	this->k_reset = get_k(SOFT_RESET_TIME, AudioSampleFrequency);

	return 0;
}

static void adsr_free(struct module *m)
{
	ggm_free(m->priv);
}

static bool adsr_process(struct module *m, float *buf[])
{
	struct adsr *this = (struct adsr *)m->priv;
	float *out = buf[0];

	if (this->state == ADSR_STATE_IDLE) {
		// zero output
		return false;
	}

	for (int i = 0; i < AudioBufferSize; i++) {
		switch (this->state) {

		case ADSR_STATE_IDLE:
			/* idle - do nothing */
			break;

		case ADSR_STATE_ATTACK:
			/* attack until 1.0 level */
			if (this->val < this->d_trigger) {
				this->val += this->ka * (1.f - this->val);
			} else {
				/* goto decay state */
				this->val = 1.f;
				this->state = ADSR_STATE_DECAY;
			}
			break;

		case ADSR_STATE_DECAY:
			/* decay until sustain level */
			if (this->val > this->s_trigger) {
				this->val += this->kd * (this->s - this->val);
			} else {
				if (this->s != 0.f) {
					/* goto sustain state */
					this->val = this->s;
					this->state = ADSR_STATE_SUSTAIN;
				} else {
					/* no sustain, goto idle state */
					this->val = 0.f;
					this->state = ADSR_STATE_IDLE;
				}
			}
			break;

		case ADSR_STATE_SUSTAIN:
			/* sustain - do nothing */
			break;

		case ADSR_STATE_RELEASE:
			/* release until idle level */
			if (this->val > this->i_trigger) {
				this->val += this->kr * (0.f - this->val);
			} else {
				/* goto idle state */
				this->val = 0.f;
				this->state = ADSR_STATE_IDLE;
			}
			break;

		case ADSR_STATE_RESET:
			/* soft reset to the idle level */
			if (this->val > this->i_trigger) {
				this->val += this->k_reset * (0.f - this->val);
			} else {
				/* goto idle state */
				this->val = 0.f;
				this->state = ADSR_STATE_IDLE;
			}
			break;

		default:
			LOG_ERR("bad adsr state %d", this->state);
			this->val = 0.f;
			this->state = ADSR_STATE_IDLE;
			break;
		}
		out[i] = this->val;
	}

	return true;
}

/******************************************************************************
 * module information
 */

static const struct port_info in_ports[] = {
	{ .name = "reset", .type = PORT_TYPE_BOOL, .func = adsr_port_reset },
	{ .name = "gate", .type = PORT_TYPE_FLOAT, .func = adsr_port_gate },
	{ .name = "attack", .type = PORT_TYPE_FLOAT, .func = adsr_port_attack },
	{ .name = "decay", .type = PORT_TYPE_FLOAT, .func = adsr_port_decay },
	{ .name = "sustain", .type = PORT_TYPE_FLOAT, .func = adsr_port_sustain },
	{ .name = "release", .type = PORT_TYPE_FLOAT, .func = adsr_port_release },
	PORT_EOL,
};

static const struct port_info out_ports[] = {
	{ .name = "out", .type = PORT_TYPE_AUDIO, },
	PORT_EOL,
};

const struct module_info env_adsr_module = {
	.name = "env.adsr",
	.in = in_ports,
	.out = out_ports,
	.alloc = adsr_alloc,
	.free = adsr_free,
	.process = adsr_process,
};

MODULE_REGISTER(env_adsr_module);

/*****************************************************************************/
