#include <mbed.h>
Ticker ticker;
volatile int ten_milliseconds = 0;
void increment_ten_milliseconds() {
    ten_milliseconds++;
    ten_milliseconds%=50000;
}
// =================================================
// * Recitation 5: SPI and Gyroscope *
// =================================================

// TODOs:
// [1] Get started with an SPI object instance and connect to the Gyroscope!
// [2] Read the XYZ axis from the Gyroscope and Visualize on the Teleplot. 
// [3] Fetching Data from the sensor via Polling vs Interrupt ?

// Define control register addresses and their configurations
#define CTRL_REG1 0x20
#define CTRL_REG1_CONFIG 0b01'10'1'1'1'1
#define CTRL_REG4 0x23
#define CTRL_REG4_CONFIG 0b0'0'01'0'00'0
#define SPI_FLAG 1
#define OUT_X_L 0x28

EventFlags flags;

void spi_cb(int event)
{
    flags.set(SPI_FLAG);                            
}

#define SCALING_FACTOR (0.0174532925199432957692236907684886f)


int main()
{
    // Initialize the SPI object with specific pins.
    SPI spi(PF_9, PF_8, PF_7, PC_1, use_gpio_ssel);
    ticker.attach(&increment_ten_milliseconds, 0.01);
    // Buffers for sending and receiving data over SPI.
    uint8_t write_buf[32], read_buf[32];
    uint8_t steady;
    // Configure SPI format and frequency.
    spi.format(8, 3);
    spi.frequency(1'000'000);

    // Configure CTRL_REG1 register.
    write_buf[0] = CTRL_REG1;
    write_buf[1] = CTRL_REG1_CONFIG;
    spi.transfer(write_buf, 2, read_buf, 2, spi_cb);
    flags.wait_all(SPI_FLAG);

    // Configure CTRL_REG4 register.
    write_buf[0] = CTRL_REG4;
    write_buf[1] = CTRL_REG4_CONFIG;
    spi.transfer(write_buf, 2, read_buf, 2, spi_cb);
    flags.wait_all(SPI_FLAG);
    uint16_t begin_time;

    float a[5] = {1.00000000, -0.48243073, 0.81005581, -0.22687555, 0.27221494};
    float b[5] = {0.13110644, 0.00000000, -0.26221288, 0.00000000, 0.13110644};

    // Initialize delay line
    float raw_delay[5] = {0};
    int16_t out_delay[5] = {0};
    int16_t avg_gy=0;
    int16_t parkinson_signal=0;
    while(1){
        
        uint16_t raw_gx, raw_gy, raw_gz;
        int16_t gx, gy, gz;
        begin_time=ten_milliseconds;
        // Prepare to read the gyroscope values starting from OUT_X_L
        write_buf[0] = OUT_X_L | 0x80 | 0x40;

        // Perform the SPI transfer to read 6 bytes of data (for x, y, and z axes)
        spi.transfer(write_buf, 7, read_buf, 7, spi_cb);
        flags.wait_all(SPI_FLAG);

        // Convert the received data into 16-bit integers for each axis
        raw_gx = (((uint16_t)read_buf[2]) << 8) | ((uint16_t) read_buf[1]);
        raw_gy = (((uint16_t)read_buf[4]) << 8) | ((uint16_t) read_buf[3]);
        raw_gz = (((uint16_t)read_buf[6]) << 8) | ((uint16_t) read_buf[5]);
        //Print the raw values for debugging 
        //printf("RAW -> \t\tgx: %d \t gy: %d \t gz: %d \t\n", raw_gx, raw_gy, raw_gz);

            //printf(">x_axis: %d|g \n", raw_gx);
            //printf(">y_axis: %d|g \n", raw_gy);
            //printf(">z_axis: %d|g \n", raw_gz);
        
        // Convert raw data to actual values using a scaling factor
        gx = ((int16_t) raw_gx) * SCALING_FACTOR;
        gy = ((int16_t) raw_gy) * SCALING_FACTOR;
        gz = ((int16_t) raw_gz) * SCALING_FACTOR;
        
        for (int i = 4; i > 0; i--) {
            raw_delay[i] = raw_delay[i - 1];
            out_delay[i] = out_delay[i - 1];
        }

        // Input sample
        raw_delay[0] = gy;

        // Compute output sample
        out_delay[0] = b[0] * raw_delay[0];
        for (int i = 1; i < 5; i++) {
            out_delay[0] += int16_t (b[i] * raw_delay[i]) ;//;- a[i] * out_delay[i]
        }

        // Print the actual values
        //("Actual data -> \t\tgx: %d \t gy: %d \t gz: %d \t\n", gx, gy, gz);
            //printf(">x_axis: %d|g \n", gx);
        printf(">y_axis: %d|g \n", abs(out_delay[0]));
            //printf(">z_axis: %d|g \n", gz);
        if (abs(gx)+abs(gz)<50){
            steady=1;
        }
        else{
            steady=0;
        }
        avg_gy=int16_t((49*avg_gy+steady*abs(out_delay[0]))/50);
        printf(">avg_gy: %d|g \n", avg_gy);
        if (avg_gy>30){
            parkinson_signal=1;
        }
        else{
            parkinson_signal=0;
        }
        //printf("parkinson_signal: %d \n", parkinson_signal);
        thread_sleep_for(50-(ten_milliseconds-begin_time)%50000*10);
    }

    }
