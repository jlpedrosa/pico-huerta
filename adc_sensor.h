
struct AcdPin {
    int GPIO_PIN;
    int ADC_NUM;
};

const AcdPin ACD_GPIO_26 = {26, 0};
const AcdPin ACD_GPIO_27 = {27, 1};

void InitializeACD();

class AcdSensor
{
    public:
        AcdSensor(AcdPin pin);
        float GetValue();
    
    private:
        AcdPin pin;
};