#include<stdio.h>
#include<rpi-rgb-led-matrix/led-matrix-c.h>

int main(int argc, char **argv){
    struct RGBLedMatrixOptions options = {0};
    struct RGBLedMatrix *ctx;
    struct LedCanvas *vbuf;
    int width, height;
    int x, y;

    options.rows = 32;
    options.chain_length = 3;

    ctx = led_matrix_create_from_options(&options, &argc, &argv);
    if (ctx == NULL)
        return 1;
    vbuf = led_matrix_create_offscreen_canvas(ctx);
    led_canvas_get_size(vbuf, &width, &height);
    while (1){
        for (y = 0; y < height; ++y){
            for (x = 0; x < width; ++x){
                led_canvas_set_pixel(vbuf, x, y, 255, 255, 255);
            }
        }
        vbuf = led_matrix_swap_on_vsync(ctx, vbuf);
    }
    led_matrix_delete(ctx);
    return 0;
}