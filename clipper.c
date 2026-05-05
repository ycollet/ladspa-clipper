/*
	clipper.c

	Copyright (C) 2002 Tim Goetze <tim@quitte.de>

	http://quitte.de/dsp/

	hard clipping, without any aliasing protection. Amplify the
	incoming signal by 30 dB and more to get good, harsh distortion.
*/
/*
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
	02111-1307, USA or point your web browser to http://www.gnu.org.
*/

#include <malloc.h>
#include <ladspa.h>

typedef struct {
    LADSPA_Data * in;
    LADSPA_Data * out;

    LADSPA_Data gain;

    LADSPA_Data clip;
    LADSPA_Data state, last;
}
clipper;

#define PROCESS(assignment) \
	register LADSPA_Data  \
			a1 = 1.f - ((clipper *) h)->clip,  \
			a2 = -1.f + ((clipper *) h)->clip / 2.f; \
	\
	register LADSPA_Data * in = ((clipper *) h)->in; \
	register LADSPA_Data * out = ((clipper *) h)->out; \
	\
	register LADSPA_Data last = ((clipper *) h)->last; \
	register LADSPA_Data state = ((clipper *) h)->state; \
	register LADSPA_Data gain = ((clipper *) h)->gain; \
	\
	int i; \
	\
	for (i = 0; i < frames; ++i) \
	{ \
		register LADSPA_Data x = in[i]; \
		\
		if (x > a1) \
			x = a1; \
		else if (x < a2) \
			x = a2; \
		 \
		state = .999f * state + x - last; \
		last = x; \
		\
		assignment; \
	} \
	\
	((clipper *) h)->last = last; \
	((clipper *) h)->state = state;

/* /////////////////////////////////////////////////////////////////////// */

static LADSPA_Handle
instantiate (const struct _LADSPA_Descriptor * desc, unsigned long sr) {
    clipper * h = (clipper *) malloc (sizeof (clipper));
    h->gain = 1.f;
    h->clip = .5f;
    return h;
}

static void
connect_port (LADSPA_Handle h, unsigned long n, LADSPA_Data * to) {
    if (n == 0)
        ((clipper *) h)->in = to;
    else if (n == 1)
        ((clipper *) h)->out = to;
}

static void
activate (LADSPA_Handle h) {
    ((clipper *) h)->last =
        ((clipper *) h)->state = 0.f;
}

static void
run (LADSPA_Handle h, unsigned long frames) {
    PROCESS (out[i] = state);
}

static void
run_adding (LADSPA_Handle h, unsigned long frames) {
    PROCESS (out[i] += gain * state);
}

static void
set_run_adding_gain (LADSPA_Handle h, LADSPA_Data gain) {
    ((clipper *) h)->gain = gain;
}

static void
cleanup (LADSPA_Handle h) {
    free (h);
}

/* /////////////////////////////////////////////////////////////////////// */

static const LADSPA_PortDescriptor
port_descriptors[] = {
    LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
    LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO,
};

static const char *
port_names[] = {
    "in",
    "out",
};

static const LADSPA_PortRangeHint
port_range_hints[] = {
    /* gaan we mieren neuken? ja wel! */
    {
        LADSPA_HINT_BOUNDED_BELOW |
        LADSPA_HINT_BOUNDED_ABOVE |
        LADSPA_HINT_DEFAULT_MIDDLE,
        -1.0, 1.0
    },
    {
        LADSPA_HINT_BOUNDED_BELOW |
        LADSPA_HINT_BOUNDED_ABOVE |
        LADSPA_HINT_DEFAULT_MIDDLE,
        -1.0, 1.0
    },
};

static const LADSPA_Descriptor
descriptor = {
    UniqueID:	1762,
Label: "clipper",
Properties: LADSPA_PROPERTY_HARD_RT_CAPABLE,
Name: "hard clipper (no antialiasing)",
Maker: "Tim Goetze <tim@quitte.de>",
Copyright: "GPL, (C) 2002 <tim@quitte.de>",

    PortCount: 2,
PortDescriptors: port_descriptors,
PortNames: port_names,
PortRangeHints: port_range_hints,

    /* methods */
instantiate: instantiate,
connect_port: connect_port,
activate: activate,
run: run,
run_adding: run_adding,
set_run_adding_gain: set_run_adding_gain,
cleanup: cleanup,
};

/* /////////////////////////////////////////////////////////////////////// */

const LADSPA_Descriptor *
ladspa_descriptor (unsigned long index) {
    return index ? 0 : &descriptor;
}

