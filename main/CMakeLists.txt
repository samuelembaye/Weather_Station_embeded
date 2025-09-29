idf_component_register(
    SRCS 
        "main.c"
        "src/i2c_driver.c"
        "src/uart_driver.c"
        # "src/pin_driver.c"
        # "src/led_strip_driver.c"
        # "src/led_strip_encoder.c" 
    INCLUDE_DIRS 
        "include"
    REQUIRES 
        driver log
)
