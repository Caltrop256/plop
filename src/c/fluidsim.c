#include "main.h"
#include "fluidsim.h"
#include "elements/elements.h"
#include "walloc.h"

void *memcpy(void *dest, const void *src, unsigned long s) {
    char *csrc = (char *)src;
    char *cdest = (char *)dest;
    while(s --> 0) cdest[s] = csrc[s];
    return dest;
}

void set_bnd(U8 b, F32 *x) {
    for (int i = 1; i < N - 1; i++) {
        x[IX(i, 0  )] = b == 2 ? -x[IX(i, 1  )] : x[IX(i, 1 )];
        x[IX(i, N-1)] = b == 2 ? -x[IX(i, N-2)] : x[IX(i, N-2)];
    }
    for (int j = 1; j < N - 1; j++) {
        x[IX(0, j)] = b == 1 ? -x[IX(1, j)] : x[IX(1, j)];
        x[IX(N-1, j)] = b == 1 ? -x[IX(N-2, j)] : x[IX(N-2, j)];
    }

    x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
    x[IX(0, N-1)] = 0.5f * (x[IX(1, N-1)] + x[IX(0, N-2)]);
    x[IX(N-1, 0)] = 0.5f * (x[IX(N-2, 0)] + x[IX(N-1, 1)]);
    x[IX(N-1, N-1)] = 0.5f * (x[IX(N-2, N-1)] + x[IX(N-1, N-2)]);
}

void lin_solve(U8 b, F32 *x, F32 *x0, F32 a, F32 c) {
    F32 cRecip = 1.0 / c;
    for (int k = 0; k < ITER; k++) {
        for (int j = 1; j < N - 1; j++) {
            for (int i = 1; i < N - 1; i++) {
                x[IX(i, j)] =
                (x0[IX(i, j)]
                + a*(    x[IX(i+1, j)]
                +x[IX(i-1, j)]
                +x[IX(i, j+1)]
                +x[IX(i, j-1)]
                )) * cRecip;
            }
        }
    set_bnd(b, x);
    }
}

void diffuse(U8 b, F32 *x, F32 *x0, F32 diff, F32 dt) {
    F32 a = dt * diff * (N - 2) * (N - 2);
    lin_solve(b, x, x0, a, 1 + 4 * a);
}

void project(F32 *velocX, F32 *velocY, F32 *p, F32 *div) {
    for (int j = 1; j < N - 1; j++) {
        for (int i = 1; i < N - 1; i++) {
        div[IX(i, j)] = -0.5f*(
            velocX[IX(i+1, j)]
            -velocX[IX(i-1, j)]
            +velocY[IX(i, j+1)]
            -velocY[IX(i, j-1)]
            )/N;
        p[IX(i, j)] = 0;
        }
    }

    set_bnd(0, div); 
    set_bnd(0, p);
    lin_solve(0, p, div, 1, 4);

    for (int j = 1; j < N - 1; j++) {
        for (int i = 1; i < N - 1; i++) {
        velocX[IX(i, j)] -= 0.5f * (  p[IX(i+1, j)]
            -p[IX(i-1, j)]) * N;
        velocY[IX(i, j)] -= 0.5f * (  p[IX(i, j+1)]
            -p[IX(i, j-1)]) * N;
        }
    }
    set_bnd(1, velocX);
    set_bnd(2, velocY);
}

F64 floor(F64 num) {
    if (num >= INT32_MAX || num <= INT64_MIN || num != num) return num;
    int64_t n = (int64_t)num;
    F64 d = (F64)n;
    if (d == num || num >= 0) return d;
    else return d - 1;
}

void advect(U8 b, F32 *d, F32 *d0, F32 *velocX, F32 *velocY, F32 dt) {
    F32 i0, i1, j0, j1;

    F32 dtx = dt * (N - 2);
    F32 dty = dt * (N - 2);

    F32 s0, s1, t0, t1;
    F32 tmp1, tmp2, x, y;

    F32 NF32 = N;
    F32 iF32, jF32;
    int i, j;

    for (j = 1, jF32 = 1; j < N - 1; j++, jF32++) { 
        for (i = 1, iF32 = 1; i < N - 1; i++, iF32++) {
        tmp1 = dtx * velocX[IX(i, j)];
        tmp2 = dty * velocY[IX(i, j)];
        x    = iF32 - tmp1; 
        y    = jF32 - tmp2;

        if (x < 0.5f) x = 0.5f; 
        if (x > NF32 + 0.5f) x = NF32 + 0.5f; 
        i0 = floor(x); 
        i1 = i0 + 1.0f;
        if (y < 0.5f) y = 0.5f; 
        if (y > NF32 + 0.5f) y = NF32 + 0.5f; 
        j0 = floor(y);
        j1 = j0 + 1.0f; 

        s1 = x - i0; 
        s0 = 1.0f - s1; 
        t1 = y - j0; 
        t0 = 1.0f - t1;

        int i0i = (int)(i0);
        int i1i = (int)(i1);
        int j0i = (int)(j0);
        int j1i = (int)(j1);

        d[IX(i, j)] = 
            s0 * (t0 * d0[IX(i0i, j0i)] + t1 * d0[IX(i0i, j1i)]) +
            s1 * (t0 * d0[IX(i1i, j0i)] + t1 * d0[IX(i1i, j1i)]);
        }
    }

    set_bnd(b, d);
}

void stepFluid(void) {    
    diffuse(1, fluid.vx0, fluid.vx, fluid.visc, fluid.dt);
    diffuse(2, fluid.vy0, fluid.vy, fluid.visc, fluid.dt);

    project(fluid.vx0, fluid.vy0, fluid.vx, fluid.vy);

    advect(1, fluid.vx, fluid.vx0, fluid.vx0, fluid.vy0, fluid.dt);
    advect(2, fluid.vy, fluid.vy0, fluid.vx0, fluid.vy0, fluid.dt);

    project(fluid.vx, fluid.vy, fluid.vx0, fluid.vy0);

    diffuse(0, fluid.s, fluid.density, fluid.diff, fluid.dt);
    advect(0, fluid.density, fluid.s, fluid.vx, fluid.vy, fluid.dt);
}

FluidSim fluid;