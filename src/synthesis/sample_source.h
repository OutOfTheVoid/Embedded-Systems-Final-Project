#ifndef SAMPLE_SOURCE_H
#define SAMPLE_SOURCE_H

typedef float (* sample_source_compute_fn_t)(float, float, void *);

typedef struct {
	float (* compute_fn)(float t, float dt, void * data);
	float last_t_compute;
	float last_val;
	void * data;
} SampleSource;

static inline void sample_source_init(SampleSource * source, sample_source_compute_fn_t compute_fn, void * data) {
	source -> compute_fn = compute_fn;
	source -> data = data;
	source -> last_val = 0.0f;
	source -> last_t_compute = - 1.0f;
}

static inline float sample_source_get(SampleSource * source, float t, float dt) {
	if (t != source -> last_t_compute) {
		source -> last_val = (* source -> compute_fn)(t, dt, source -> data);
		source -> last_t_compute = t;
	}
	return source -> last_val;
}

#endif
