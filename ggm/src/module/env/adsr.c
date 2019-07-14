/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "module.h"

/******************************************************************************
 * private state
 */

enum adsr_state {
	ADSR_STATE_IDLE = 0, /* initial state */
	ADSR_STATE_ATTACK,
	ADSR_STATE_DECAY,
	ADSR_STATE_SUSTAIN,
	ADSR_STATE_RELEASE,
};

struct adsr_env {
	enum adsr_state state;          /* envelope state */
	float s;                        /* sustain level */
	float ka;                       /* attack constant */
	float kd;                       /* decay constant */
	float kr;                       /* release constant */
	float d_trigger;                /* attack->decay trigger level */
	float s_trigger;                /* decay->sustain trigger level */
	float i_trigger;                /* release->idle trigger level */
	float val;                      /* output value */
};

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

/* envelope gate, attack(>0) or release(=0) */
static void adsr_port_gate(struct module *m, struct event *e)
{
	struct adsr_env *env = (struct adsr_env *)m->priv;
	float gate = event_get_float(e);
	char tmp[64];

	LOG_INF("gate %s", log_strdup(ftoa(gate, tmp)));
	if (gate != 0.f) {
		/* enter the attack segment */
		env->state = ADSR_STATE_ATTACK;
	} else {
		/* enter the release segment */
		if (env->state != ADSR_STATE_IDLE) {
			if (env->kr == 1.f) {
				/* no release - goto idle */
				env->val = 0.f;
				env->state = ADSR_STATE_IDLE;
			} else {
				env->state = ADSR_STATE_RELEASE;
			}
		}
	}
}

/* attack time (secs) */
static void adsr_port_attack(struct module *m, struct event *e)
{
	struct adsr_env *env = (struct adsr_env *)m->priv;
	float attack = clampf_lo(event_get_float(e), 0.f);
	char tmp[64];

	LOG_INF("set attack time %s secs", log_strdup(ftoa(attack, tmp)));
	env->ka = get_k(attack, AudioSampleFrequency);
}

/* decay time (secs) */
static void adsr_port_decay(struct module *m, struct event *e)
{
	struct adsr_env *env = (struct adsr_env *)m->priv;
	float decay = clampf_lo(event_get_float(e), 0.f);
	char tmp[64];

	LOG_INF("set decay time %s secs", log_strdup(ftoa(decay, tmp)));
	env->kd = get_k(decay, AudioSampleFrequency);
}

/* sustain level 0..1 */
static void adsr_port_sustain(struct module *m, struct event *e)
{
	struct adsr_env *env = (struct adsr_env *)m->priv;
	float sustain = clampf(event_get_float(e), 0.f, 1.f);
	char tmp[64];

	LOG_INF("set sustain level %s", log_strdup(ftoa(sustain, tmp)));
	env->s = sustain;
	env->d_trigger = 1.f - LEVEL_EPSILON;
	env->s_trigger = sustain + (1.f - sustain) * LEVEL_EPSILON;
	env->i_trigger = sustain * LEVEL_EPSILON;
}

/* release time (secs) */
static void adsr_port_release(struct module *m, struct event *e)
{
	struct adsr_env *env = (struct adsr_env *)m->priv;
	float release = clampf_lo(event_get_float(e), 0.f);
	char tmp[64];

	LOG_INF("set release time %s secs", log_strdup(ftoa(release, tmp)));
	env->kr = get_k(release, AudioSampleFrequency);
}

/******************************************************************************
 * module functions
 */

static int adsr_init(struct module *m)
{
	/* allocate the private data */
	struct adsr_env *env = k_calloc(1, sizeof(struct adsr_env));

	if (env == NULL) {
		LOG_ERR("could not allocate private data");
		return -1;
	}

	m->priv = (void *)env;
	return 0;
}

static void adsr_stop(struct module *m)
{
	k_free(m->priv);
}

static struct module **adsr_child(struct module *m)
{
	/* no children */
	return NULL;
}

static bool adsr_process(struct module *m, float *buf[])
{
	struct adsr_env *env = (struct adsr_env *)m->priv;
	float *out = buf[0];

	if (env->state == ADSR_STATE_IDLE) {
		// zero output
		return false;
	}

	for (int i = 0; i < AudioBufferSize; i++) {
		switch (env->state) {
		case ADSR_STATE_ATTACK:
			/* attack until 1.0 level */
			if (env->val < env->d_trigger) {
				env->val += env->ka * (1.f - env->val);
			} else {
				/* goto decay state */
				env->val = 1.f;
				env->state = ADSR_STATE_DECAY;
			}
			break;
		case ADSR_STATE_DECAY:
			/* decay until sustain level */
			if (env->val > env->s_trigger) {
				env->val += env->kd * (env->s - env->val);
			} else {
				if (env->s != 0.f) {
					/* goto sustain state */
					env->val = env->s;
					env->state = ADSR_STATE_SUSTAIN;
				} else {
					/* no sustain, goto idle state */
					env->val = 0.f;
					env->state = ADSR_STATE_IDLE;
				}
			}
			break;
		case ADSR_STATE_SUSTAIN:
			/* sustain - do nothing */
			env->val = env->s;
			break;
		case ADSR_STATE_RELEASE:
			/* release until idle level */
			if (env->val > env->i_trigger) {
				env->val += env->kr * (0.f - env->val);
			} else {
				/* goto idle state */
				env->val = 0.f;
				env->state = ADSR_STATE_IDLE;
			}
			break;
		default:
			LOG_ERR("bad adsr state %d", env->state);
			env->state = ADSR_STATE_IDLE;
			break;
		}
		out[i] = env->val;
	}

	return true;
}

/******************************************************************************
 * module information
 */

const static struct port_info in_ports[] = {
	{ .name = "gate", .type = PORT_TYPE_FLOAT, .func = adsr_port_gate },
	{ .name = "attack", .type = PORT_TYPE_FLOAT, .func = adsr_port_attack },
	{ .name = "decay", .type = PORT_TYPE_FLOAT, .func = adsr_port_decay },
	{ .name = "sustain", .type = PORT_TYPE_FLOAT, .func = adsr_port_sustain },
	{ .name = "release", .type = PORT_TYPE_FLOAT, .func = adsr_port_release },
};

static const struct port_info out_ports[] = {
	{ .name = "out", .type = PORT_TYPE_AUDIO, },
};

const struct module_info adsr_module = {
	.name = "adsr",
	.in = in_ports,
	.out = out_ports,
	.init = adsr_init,
	.stop = adsr_stop,
	.child = adsr_child,
	.process = adsr_process,
};

MODULE_REGISTER(adsr_module);

/*****************************************************************************/
