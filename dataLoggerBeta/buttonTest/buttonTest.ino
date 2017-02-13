/**
* @Author: Yichen Xu
* @Date:   12-Feb-2017 01:02:28
* @Email:  ichi@u.northwestern.edu
* @Last modified by:   Yichen Xu
* @Last modified time: 12-Feb-2017 01:02:35
*/



void setup() {
    pinMode(30, INPUT);
    Serial.begin(9600);
}

void loop() {
    Serial.println(digitalRead(30));
}
