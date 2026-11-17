#include <gte.h>


AFFINE create_set_affine(const TRANSFORM *transform){

    load_transform(transform);

    AFFINE out = {get_vector(),get_matrix()};

    return out;
}


