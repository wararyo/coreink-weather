#ifndef _BATTERY_H_
#define _BATTERY_H_

#include <M5CoreInk.h>
#include <esp_adc_cal.h>

// Reference: https://github.com/ksasao/ImageToCoreInk/blob/main/sample/arduino/sample_image/sample_image.ino

float getBatVoltage()
{
    analogSetPinAttenuation(35,ADC_11db);
    esp_adc_cal_characteristics_t *adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 3600, adc_chars);
    uint16_t ADCValue = analogRead(35);
    
    uint32_t BatVolmV  = esp_adc_cal_raw_to_voltage(ADCValue,adc_chars);
    float BatVol = float(BatVolmV) * 25.1 / 5.1 / 1000;
    return BatVol;
}

int getBatCapacity(){
    // Simple implementation
    // see https://www.maximintegrated.com/jp/design/technical-documents/app-notes/3/3958.html
    // 4.15 = 100%, 3.40 = 0%
    const float maxVoltage = 4.15;
    const float minVoltage = 3.40;
    int cap = (int)(100.0 * (getBatVoltage() - minVoltage) / (maxVoltage - minVoltage));
    if(cap > 100){
      cap = 100;
    }
    if(cap < 0){
      cap = 0;
    }
    return cap;
}

#endif _BATTERY_H_