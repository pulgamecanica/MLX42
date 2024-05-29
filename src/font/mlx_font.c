/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   mlx_font.c                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: W2Wizard <main@w2wizard.dev>                 +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/22 12:01:37 by W2Wizard      #+#    #+#                 */
/*   Updated: 2022/06/27 19:53:36 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "font.h"
#include "MLX42/MLX42_Int.h"
#include "MLX42/MLX42.h"

//= Private =//

/**
 * Does the actual copying of pixels form the atlas buffer to the
 * image buffer.
 * 
 * Skips any non-printable characters.
 * 
 * @param image The image to draw on.
 * @param texture The font_atlas.
 * @param texoffset The character texture X offset.
 * @param imgoffset The image X offset.
 */
static void mlx_draw_char(mlx_image_t* image, int32_t texoffset, int32_t imgoffset)
{
	if (texoffset < 0)
		return;

	char* pixelx;
	uint8_t* pixeli;
	for (uint32_t y = 0; y < FONT_HEIGHT; y++)
	{
		pixelx = &font_atlas.pixels[(y * font_atlas.width + texoffset) * BPP];
		pixeli = image->pixels + ((y * image->width + imgoffset) * BPP);
		memcpy(pixeli, pixelx, FONT_WIDTH * BPP);
	}
}

//= Public =//

const mlx_texture_t* mlx_get_font(void)
{
    return ((const mlx_texture_t*)&font_atlas);
}

int32_t mlx_get_texoffset(char c)
{
    const bool _isprint = isprint(c);

    // NOTE: Cheesy branchless operation :D
    // +2 To skip line separator in texture
    return (-1 * !_isprint + ((FONT_WIDTH + 2) * (c - 32)) * _isprint);
}

mlx_image_t* mlx_put_string(mlx_t* mlx, const char* str, int32_t x, int32_t y)
{
	MLX_NONNULL(mlx);
	MLX_NONNULL(str);

	mlx_image_t* strimage;
	const size_t len = strlen(str);
	if (len > MLX_MAX_STRING)
		return ((void*)mlx_error(MLX_STRTOOBIG));	
	if (!(strimage = mlx_new_image(mlx, len * FONT_WIDTH, FONT_HEIGHT)))
		return (NULL);

	// Draw the text itself
	int32_t imgoffset = 0;
	for (size_t i = 0; i < len; i++, imgoffset += FONT_WIDTH)
		mlx_draw_char(strimage, mlx_get_texoffset(str[i]), imgoffset);

	if (mlx_image_to_window(mlx, strimage, x, y) == -1)
		return (mlx_delete_image(mlx, strimage), NULL);
	return (strimage);
}

// PULGA EDIT
static int32_t ft_pixel(int32_t r, int32_t g, int32_t b, int32_t a) {
  return (r << 24 | g << 16 | b << 8 | a);
}

static int32_t mlx_get_pixel(mlx_image_t* image, uint32_t x, uint32_t y) {
  if (x > image->width || y > image->height)
    return 0xFFFFFFFF;
  uint8_t* pixelstart = image->pixels + (y * image->width + x) * BPP;
  return ft_pixel(*(pixelstart), *(pixelstart + 1),
    * (pixelstart + 2), *(pixelstart + 3));
}

static int put_pixel_valid(mlx_image_t* img, uint32_t x, uint32_t y) {
    return (x < img->width && y < img->height && x > 0 && y > 0);
}

static void  put_img_to_img(mlx_image_t* dst, mlx_image_t* src, int x, int y, int color) {
  unsigned i;
  unsigned j;

  i = 0;
  while(i < src->width) {
    j = 0;
    while (j < src->height) {
    	if (put_pixel_valid(src, i, j)) {
    		int tmp_color = mlx_get_pixel(src, x + i, y + j);
        if (tmp_color == 0xFFFFFFFF)
      		mlx_put_pixel(dst, x + i, y + j, color);
      	}
      j++;
    }
    i++;
  }
}

void ft_mlx_put_string(mlx_t* mlx, mlx_image_t * img, const char* str, int32_t x, int32_t y, int color) {
  mlx_image_t* strimage;
  const size_t len = strlen(str);
  if (!(strimage = mlx_new_image(mlx, len * FONT_WIDTH, FONT_HEIGHT)))
    return ;

  // Draw the text itself
  int32_t imgoffset = 0;
  for (size_t i = 0; i < len; i++, imgoffset += FONT_WIDTH)
    mlx_draw_char(strimage, mlx_get_texoffset(str[i]), imgoffset);
  // Put the image to another image
  put_img_to_img(img, strimage, x, y, color);
  mlx_delete_image(mlx, strimage);
}
