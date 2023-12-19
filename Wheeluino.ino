/*
 * @file Wheeluino.ino
 * @brief An Arduino controlled Wheel-O.
 * @author Trent M Wyatt
 * @date 2023-12-19
 * @version 1.0
 */

#include <Arduino.h>
#include <Servo.h>

#define SERVOPIN 3
#define RESTART_MS 30000UL

/**
 * @struct Wheeluino
 * @brief Structure representing the Wheel-O and its control.
 */
class Wheeluino {
    double const slow = 11.0;           ///< Slow speed constant.
    double const fast = 6.6;            ///< Fast speed constant.
    double const pause_min = 23.0;      ///< Minimum pause duration constant.
    double const pause_factor = 0.47;   ///< Pause factor constant.
    double const speed_factor = 0.94;   ///< Speed factor constant.
    int const pos_max = 90;             ///< Maximum position constant.
    int const pos_min = 20;             ///< Minimum position constant.

    Servo servo;    ///< Servo motor object.

    int pin;        ///< Pin to which the servo is connected.

    double speed;   ///< Current speed of the Wheel-O.

    double pause;   ///< Pause duration at each end.

    int pos;        ///< Current position of the servo.

public:

    /**
     * @brief Constructor for the Wheeluino structure.
     * @param _pin The pin to which the servo is connected.
     */
    Wheeluino(int const _pin) :
        pin(_pin),
        speed(slow),
        pause(90.0),
        pos(((pos_max - pos_min) / 2) + pos_min) // Default position in the center range.
    {
    }

    /**
     * @brief Initializes the Wheeluino by attaching the servo and starting over.
     */
    void begin() {
        servo.attach(pin);
        start_over();
    }

    /**
     * @brief Stops the Wheeluino by detaching the servo and settling down.
     */
    void end() {
        servo.detach();
        settle_down();
    }

    /**
     * @brief Raises the end of the Wheel-O until it reaches the maximum position.
     */
    void raise() {
        while (pos < pos_max) {
            pos++;
            servo.write(pos);
            delay(speed);
        }
    }

    /**
     * @brief Lowers the end of the Wheel-O until it reaches the minimum position.
     */
    void lower() {
        while (pos > pos_min) {
            pos--;
            servo.write(pos);
            delay(speed);
        }
    }

    /**
     * @brief Points the Wheel-O down and waits for it to settle.
     */
    void settle_down() {
        pos = pos_min;
        servo.write(pos);
        delay(19000);
    }

    /**
     * @brief Speeds up the Wheel-O from a stopped position.
     */
    void speed_up() {
        pause = 90.0;
        speed = slow;

        while (speed > fast) {
            speed *= speed_factor;

            raise();
            delay(pause * speed);
            pause *= (pause > pause_min) ? pause_factor : 1.0;

            lower();
            delay(pause * speed);
            pause *= (pause > pause_min) ? pause_factor : 1.0;
        }
    }

    /**
     * @brief Stops and restarts the Wheel-O.
     */
    void start_over() {
        settle_down();
        speed_up();
    }

    /**
     * @brief Executes a run sequence for the Wheel-O.
     */
    void run() {
        raise();
        delay(pause * speed);

        lower();
        delay(pause * speed);
    }
};

Wheeluino wheelo(SERVOPIN);
uint32_t start_time;

/**
 * @brief Arduino setup function.
 */
void setup() {
    Serial.begin(115200);

    wheelo.begin();
    wheelo.start_over();

    start_time = millis();
}

/**
 * @brief Arduino main loop function.
 */
void loop() {
    wheelo.run();

    if (millis() - start_time >= RESTART_MS) {
        wheelo.start_over();
        start_time = millis();
    }
}
