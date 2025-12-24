#include "color.h"
#include "defines.h"
#include "nvmc.h"
#include <string.h>

#if defined(ESTC_USB_CLI_ENABLED) && ESTC_USB_CLI_ENABLED
#include "cli.h"

#include "nrf_cli.h"
#include "nrf_cli_cdc_acm.h"

#include "app_usbd.h"
#include "app_usbd_cdc_acm.h"
#include "app_usbd_serial_num.h"
#include "app_usbd_core.h"
#include "nrf_delay.h"

#ifndef USBD_POWER_DETECTION
#define USBD_POWER_DETECTION true
#endif

extern volatile hsv_color_t current_hsv;
extern volatile color_palette_t palette;

NRF_CLI_CDC_ACM_DEF(m_cli_cdc_acm_transport);
NRF_CLI_DEF(m_cli_cdc_acm,
            "mkass420_nrf52 > ",
            &m_cli_cdc_acm_transport.transport,
            '\n',
            4);

static void usbd_user_ev_handler(app_usbd_event_type_t event){
    switch (event){
        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
            break;
        case APP_USBD_EVT_POWER_DETECTED:
            if(!nrf_drv_usbd_is_enabled()){
                app_usbd_enable();
            }
            break;
        case APP_USBD_EVT_POWER_REMOVED:
            app_usbd_stop();
            break;
        case APP_USBD_EVT_POWER_READY:
            app_usbd_start();
            break;
        default:
            break;
    }
}

static void cmd_set_rgb(nrf_cli_t const * p_cli, size_t argc, char ** argv){
    if(argc != 4){
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Usage: RGB <r> <g> <b>\n");
        return;
    }
    uint8_t r = atoi(argv[1]);
    uint8_t g = atoi(argv[2]);
    uint8_t b = atoi(argv[3]);
    hsv_color_t color = rgb_to_hsv((rgb_color_t){r, g, b});
    if(isValidHSV(&color)){
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Color set to R=%d G=%d B=%d\n", r, g, b);
        current_hsv = color;
        apply_color(&current_hsv);
        write_color(&current_hsv);
    }
    else{
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "<r>, <g>, <b> should be in range 0-255\n");
    }
}

static void cmd_set_hsv(nrf_cli_t const * p_cli, size_t argc, char ** argv){
    if(argc != 4){
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Usage: HSV <h> <s> <v>\n");
        return;
    }
    uint16_t h = atoi(argv[1]);
    uint8_t  s = atoi(argv[2]);
    uint8_t  v = atoi(argv[3]);
    hsv_color_t color = {{h, s, v, "\0"}};
    if(isValidHSV(&color)){
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Color set to H=%d S=%d V=%d\n", h, s, v);
        current_hsv = color;
        apply_color(&current_hsv);
        write_color(&current_hsv);
    }
    else{
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "<h> should be in range 0-360; <s>, <v> should be in range 0-100\n");
    }
}

static void cmd_add_rgb_color(nrf_cli_t const * p_cli, size_t argc, char ** argv){
    if(argc != 5){
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Usage: add_rgb_color <r> <g> <b> <color_name>\n");
        return;
    }
    uint8_t r = atoi(argv[1]);
    uint8_t g = atoi(argv[2]);
    uint8_t b = atoi(argv[3]);
    char name[MAX_COLOR_NAME_SIZE] = {0};
    strncpy(name, argv[4], MAX_COLOR_NAME_SIZE);
    if(!isValidColorName(name)){
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "<color_name> should not exceed %d characters\n", MAX_COLOR_NAME_SIZE - 1);
        return;
    }
    hsv_color_t color = rgb_to_hsv((rgb_color_t){r, g, b});
    strncpy(color.color_name, name, MAX_COLOR_NAME_SIZE);
    color_palette_t new_palette = palette;
    
    if(isValidHSV(&color)){
        if(add_color_to_palette(&color, &new_palette)){
            nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Added color %s R=%d G=%d B=%d to palette\n", name, r, g, b);
            palette = new_palette;
            write_palette(&palette);
        }
        else{
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s is already added or number of added colors exceeds %d\n", name, MAX_COLORS);
            return;
        }
    }
    else{
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "<r>, <g>, <b> should be in range 0-255\n");
    }
}

static void cmd_add_hsv_color(nrf_cli_t const * p_cli, size_t argc, char ** argv){
    if(argc != 5){
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Usage: add_hsv_color <h> <s> <v> <color_name>\n");
        return;
    }
    uint8_t h = atoi(argv[1]);
    uint8_t s = atoi(argv[2]);
    uint8_t v = atoi(argv[3]);
    char name[MAX_COLOR_NAME_SIZE] = {0};
    strncpy(name, argv[4], MAX_COLOR_NAME_SIZE);
    if(!isValidColorName(name)){
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "<color_name> should not exceed %d characters\n", MAX_COLOR_NAME_SIZE - 1);
        return;
    }
    hsv_color_t color = {{h, s, v, "\0"}};
    strncpy(color.color_name, name, MAX_COLOR_NAME_SIZE);
    color_palette_t new_palette = palette;
    
    if(isValidHSV(&color)){
        if(add_color_to_palette(&color, &new_palette)){
            nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Added color %s H=%d S=%d V=%d to palette\n", name, h, s, v);
            palette = new_palette;
            write_palette(&palette);
        }
        else{
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s is already added or number of added colors exceeds %d\n", name, MAX_COLORS);
            return;
        }
    }
    else{
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "<h> should be in range 0-360; <s>, <v> should be in range 0-100\n");
    }
}

static void cmd_add_current_color(nrf_cli_t const * p_cli, size_t argc, char ** argv){
    if(argc != 2){
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Usage: add_current_color <color_name>\n");
        return;
    }
    char name[MAX_COLOR_NAME_SIZE] = {0};
    strncpy(name, argv[1], MAX_COLOR_NAME_SIZE);
    if(!isValidColorName(name)){
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "<color_name> should not exceed %d characters\n", MAX_COLOR_NAME_SIZE - 1);
        return;
    }
    hsv_color_t color = current_hsv;
    strncpy(color.color_name, name, MAX_COLOR_NAME_SIZE);
    color_palette_t new_palette = palette;
    
    if(isValidHSV(&color)){
        if(add_color_to_palette(&color, &new_palette)){
            nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Added color %s H=%d S=%d V=%d to palette\n", name, current_hsv.hue, current_hsv.sat, current_hsv.val);
            palette = new_palette;
            write_palette(&palette);
        }
        else{
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s is already added or number of added colors exceeds %d\n", name, MAX_COLORS);
            return;
        }
    }
    else{
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "<h> should be in range 0-360; <s>, <v> should be in range 0-100\n");
    }
}

static void cmd_del_color(nrf_cli_t const * p_cli, size_t argc, char ** argv){
    if(argc != 2){
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Usage: del_color <color_name>\n");
        return;
    }
    char name[MAX_COLOR_NAME_SIZE] = {0};
    strncpy(name, argv[1], MAX_COLOR_NAME_SIZE);
    if(!isValidColorName(name)){
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "<color_name> should not exceed %d characters\n", MAX_COLOR_NAME_SIZE - 1);
        return;
    }
    color_palette_t new_palette = palette;
    if(remove_color_from_palette(name, &new_palette)){
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Deleted color %s from palette\n", name);
        palette = new_palette;
        write_palette(&palette);
    }
    else{
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s does not exist\n", name);
    }
}

static void cmd_apply_color(nrf_cli_t const * p_cli, size_t argc, char ** argv){
    if(argc != 2){
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Usage: apply_color <color_name>\n");
        return;
    }
    char name[MAX_COLOR_NAME_SIZE] = {0};
    strncpy(name, argv[1], MAX_COLOR_NAME_SIZE);
    if(!isValidColorName(name)){
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "<color_name> should not exceed %d characters\n", MAX_COLOR_NAME_SIZE - 1);
        return;
    }
    color_palette_t new_palette = palette;
    int pos = palette_find_name(name, &new_palette);
    if(pos != -1){
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Applied color %s from palette\n", name);
        current_hsv = palette.colors[pos];
        apply_color(&current_hsv);
    }
    else{
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s does not exist\n", name);
    }
}

static void cmd_list_colors(nrf_cli_t const * p_cli, size_t argc, char ** argv){
    if(argc != 1){
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Usage: list_colors\n");
        return;
    }
    if(palette.color_count == 0){
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Palette is empty\n");
        return;
    }
    for(int i = 0; i < palette.color_count; i++){
        hsv_color_t hsv = palette.colors[i];
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "%s H=%d S=%d V=%d\n", hsv.color_name, hsv.hue, hsv.sat, hsv.val);
    }
}

static void cmd_help(nrf_cli_t const * p_cli, size_t argc, char ** argv){
    if(argc != 1){
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Usage: help\n");
        return;
    }
    
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "\nAvailable commands:\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "===================\n\n");
    
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "RGB <r> <g> <b>\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Set current color using RGB values\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Parameters: r (0-255), g (0-255), b (0-255)\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Example: RGB 255 0 128\n\n");
    
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "HSV <h> <s> <v>\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Set current color using HSV values\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Parameters: h (0-360), s (0-100), v (0-100)\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Example: HSV 180 100 50\n\n");
    
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "add_rgb_color <r> <g> <b> <color_name>\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Add RGB color to palette with a name\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Parameters: r (0-255), g (0-255), b (0-255), name (max %d chars)\n", MAX_COLOR_NAME_SIZE - 1);
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Example: add_rgb_color 255 0 0 red\n\n");
    
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "add_hsv_color <h> <s> <v> <color_name>\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Add HSV color to palette with a name\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Parameters: h (0-360), s (0-100), v (0-100), name (max %d chars)\n", MAX_COLOR_NAME_SIZE - 1);
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Example: add_hsv_color 120 100 50 green\n\n");
    
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "add_current_color <color_name>\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Add current color to palette with a name\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Parameters: name (max %d chars)\n", MAX_COLOR_NAME_SIZE - 1);
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Example: add_current_color mycolor\n\n");
    
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "del_color <color_name>\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Remove color from palette\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Parameters: name (max %d chars)\n", MAX_COLOR_NAME_SIZE - 1);
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Example: del_color red\n\n");
    
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "apply_color <color_name>\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Set current color from palette\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Parameters: name (max %d chars)\n", MAX_COLOR_NAME_SIZE - 1);
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Example: apply_color blue\n\n");
    
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "list_colors\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  List all colors in palette\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Example: list_colors\n\n");
    
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "help\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Display this help message\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "  Example: help\n\n");
    
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Note: Maximum %d colors can be stored in palette\n", MAX_COLORS);
}

NRF_CLI_CMD_REGISTER(RGB, NULL, "Set current color using RGB values", cmd_set_rgb);
NRF_CLI_CMD_REGISTER(HSV, NULL, "Set current color using HSV values", cmd_set_hsv);
NRF_CLI_CMD_REGISTER(add_rgb_color, NULL, "Add RGB color to palette", cmd_add_rgb_color);
NRF_CLI_CMD_REGISTER(add_hsv_color, NULL, "Add HSV color to palette", cmd_add_hsv_color);
NRF_CLI_CMD_REGISTER(add_current_color, NULL, "Add current color to palette", cmd_add_current_color);
NRF_CLI_CMD_REGISTER(del_color, NULL, "Remove color from palette", cmd_del_color);
NRF_CLI_CMD_REGISTER(apply_color, NULL, "Set current color from palette", cmd_apply_color);
NRF_CLI_CMD_REGISTER(list_colors, NULL, "List all colors in palette", cmd_list_colors);
NRF_CLI_CMD_REGISTER(help, NULL, "Display help for all commands", cmd_help);

void cli_init(void)
{
    ret_code_t ret;

    ret = nrf_cli_init(&m_cli_cdc_acm, NULL, true, true, NRF_LOG_SEVERITY_INFO);
    APP_ERROR_CHECK(ret);

    static const app_usbd_config_t usbd_config = {
        .ev_state_proc = usbd_user_ev_handler
    };

    app_usbd_serial_num_generate();
    ret = app_usbd_init(&usbd_config);
    APP_ERROR_CHECK(ret);

    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&nrf_cli_cdc_acm);
    ret = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(ret);

    ret = app_usbd_power_events_enable();
    APP_ERROR_CHECK(ret);

    ret = nrf_cli_start(&m_cli_cdc_acm);
    APP_ERROR_CHECK(ret);
}

void cli_process(void)
{
    nrf_cli_process(&m_cli_cdc_acm);
}

#else
void cli_init(){};
void cli_process(){};
#endif
