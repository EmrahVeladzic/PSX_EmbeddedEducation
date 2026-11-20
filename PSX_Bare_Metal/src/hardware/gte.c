#include <gte.h>

AFFINE get_affine(void){

    AFFINE out = {get_vector(),get_matrix()};
    return out;
}


AFFINE create_set_affine(const TRANSFORM *transform){

    load_transform(transform);

    AFFINE out = get_affine();
    return out;
}

void load_affine(const AFFINE *aff){

    load_vector(&aff->translation);

    load_matrix(&aff->rotation_scale);

}


