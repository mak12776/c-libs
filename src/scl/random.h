
#define random_init() srand((time(NULL) % 300) & (time(NULL) / 100))

#define random() ((rand() & rand()) | (rand() & rand()))

#define random_double() ((double)random() / (RAND_MAX + 1.0))

#define random_range(min, max) \
    ((min) + (random_double() * (double)((max) - (min))))

#define random_choice(a, b) (random_double() >= 0.5 ? a : b)
