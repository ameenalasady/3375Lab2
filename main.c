#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030

#define BUTTONS 0xFF200050
#define SWITCH 0xFF200040

#define TIMER 0xFFFEC600

// initializes timer to 'interval'
void set_timer(int interval)
{
    (*(int *)TIMER) = interval;
}
// starts the timer counting, clears previous timeout flag
void start_timer()
{
    (*(int *)(TIMER + 0x0C)) = 0b01;
    (*(int *)(TIMER + 0x08)) = 0b0001;
}
// stops timer
void stop_timer()
{
    (*(int *)(TIMER + 0x08)) = 0b0000;
}
// returns 1 if timer is still counting, 0 if timer is done
int check_timer()
{
    if (((*(int *)(TIMER + 0x0C)) & 0x1) == 0x1)
    {
        return (0);
    }
    else

    {
        return (1);
    }
}

void wait_for_timer()
{
    volatile int checker = 1;
    // Wait for timer to finish counting
    while (checker)
    {
        checker = check_timer();
    }
}

int main()
{
    int hex_code[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
    int startButton;
    int stopButton;
    int lapButton;
    int clearButton;
    int status = 1;
    int count = 0;
    int storedCount = 0;
    int switchValue = 0;
    int minutes = 0;
    while (1)
    {

        startButton = ((*(int *)BUTTONS) & 0b1);

        stopButton = ((*(int *)BUTTONS) & 0b10);

        lapButton = ((*(int *)BUTTONS) & 0b100);

        clearButton = ((*(int *)BUTTONS) & 0b1000);

        switchValue = (*(int *)SWITCH) & 0b1;

        // printf("%d, %d, %d, %d", startButton, stopButton, lapButton, clearButton);

        if (startButton != 0)
        {
            status = 1;
        }

        if (stopButton != 0)
        {
            status = 0;
        }

        if (lapButton != 0)
        {
            storedCount = count;
        }

        if (clearButton != 0)
        {
            count = 0;
        }

        minutes = count / 6000;

        set_timer(2000000);

        start_timer();

        wait_for_timer();

        if (switchValue == 0)
        {
            (*(int *)HEX3_HEX0_BASE) = hex_code[count % 10] |
                                       (hex_code[(count / 10) % 10] << 8) |
                                       (hex_code[(count / 100) % 10] << 16) |
                                       (hex_code[(count / 1000) % 6] << 24);

            (*(int *)HEX5_HEX4_BASE) = hex_code[minutes % 10] |
                                       (hex_code[(minutes / 10) % 10] << 8);
        }
        else
        {
            (*(int *)HEX3_HEX0_BASE) = hex_code[storedCount % 10] |
                                       (hex_code[(storedCount / 10) % 10] << 8) |
                                       (hex_code[(storedCount / 100) % 10] << 16) |
                                       (hex_code[(storedCount / 1000) % 10] << 24);

            (*(int *)HEX5_HEX4_BASE) = hex_code[(storedCount / 6000) % 10] |
                                       (hex_code[((storedCount / 6000) / 10) % 10] << 8);
        }

        if (status == 1)
        {
            count++;
        }
    }
}