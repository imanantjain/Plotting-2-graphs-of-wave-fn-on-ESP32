//Anant Jain MCS20005 LAB 12 COMBINED fn Plotting
#include <math.h>
#define PLOT
#define PLOT_TASK_STACK_SIZE      4000  
#define INFERENCE_TASK_STACK_SIZE 4000  
#define NUM_Q_ITEMS 10
#define BUILT_IN_LED 2

extern float predict(float x);
extern float s_predict(float s_x);
void plot(float y_actual, float y_pred);
void s_plot(float y_actual, float y_pred);

void* a;
TaskHandle_t inferenceTaskHandle;
TaskHandle_t S_inferenceTaskHandle;
QueueHandle_t xQHandle;

int inferenceTaskParam = 0x1234;
int S_inferenceTaskParam = 0x5678;


typedef struct{
  float y_actual;
  float y_pred;
  int flag;
} Y_VALUES;

int inference_count = 0;     // Perform Inference using the model with values of x ranging from 0 to 2pi Radians
const int kInferencesPerCycle = 1000;  // Number of samples in one cycle 
const float kXrange = 2*3.14159265359f;   // Full sine wave not 2pi

void setup() {
    pinMode(BUILT_IN_LED, OUTPUT);  
    Serial.begin(500000);
    delay(500);
    #ifdef PLOT    
    Serial.println("Actual Predicted");
    #endif
    
    xQHandle = xQueueCreate(NUM_Q_ITEMS, sizeof(Y_VALUES));

    //xTaskCreatePinnedToCore(plotTask,"PlotTask",PLOT_TASK_STACK_SIZE,(void *)&plotTaskParam,2,&plotTaskHandle,0);                    
    xTaskCreatePinnedToCore(inferenceTask,"InferenceTask",INFERENCE_TASK_STACK_SIZE,(void *)&inferenceTaskParam,2,&inferenceTaskHandle,1);                     /* Should always run on Core 1, which is Application Core */
    xTaskCreatePinnedToCore(S_inferenceTask,"S_InferenceTask",INFERENCE_TASK_STACK_SIZE,(void *)&S_inferenceTaskParam,2,&S_inferenceTaskHandle,0);                     /* Should always run on Core 1, which is Application Core */

} 

void loop() { 
  digitalWrite(BUILT_IN_LED, HIGH);   
  delay(1000);                        
  digitalWrite(BUILT_IN_LED, LOW);    
  delay(1000);     
  }
   void inferenceTask(void * parameter) {
        while(1){
        inferenceEngine();
        delay(10);
      }} // end of inferenceTask()

     void S_inferenceTask(void * parameter) {
        while(1){
        delay(10);
        S_inferenceEngine();
       }}
    
// Inference related variables
float x_val;
Y_VALUES y_vals;

void inferenceEngine(void) {
   float position = static_cast<float>(inference_count) /
                     static_cast<float>(kInferencesPerCycle);
   x_val = position * kXrange;
   y_vals.y_actual = (float)(5 * pow((double)sin(x_val), 2.0) 
                              + 4 * cos(x_val));
   y_vals.y_pred   = predict(x_val);
    
    y_vals.y_actual *= 3;
    y_vals.y_pred *= 3;
    y_vals.flag = 1;
    
    bool res =  xQueueSend(xQHandle, &y_vals, pdMS_TO_TICKS(2000));
    if (!res) { printf("MsgQ: Failed to send an item.\n"); }
    inference_count++;
    if (inference_count >= kInferencesPerCycle) {inference_count = 0;}
      
    #ifndef PLOT
        delay(1000);
        Serial.print("inferenceTask(): Executing on core ");
        Serial.println(xPortGetCoreID());
    #endif
    plotTask(a);
} // end of inferenceEngine()


void plotTask(void* parameter) {
  Y_VALUES rxY_vals;
  bool res;
    #ifndef PLOT
       Serial.print("Entering plotTask. It is running on the core ");
       Serial.println(xPortGetCoreID());
       Serial.printf("plotTask: Param received is %d\n", *(int*)parameter);
    #endif

 // while(1)
 {

    #ifndef PLOT    
    delay(1000);
    Serial.print("plotTask running on core: ");
    Serial.println(xPortGetCoreID());
    #endif
    
    res = xQueueReceive(xQHandle, &rxY_vals, pdMS_TO_TICKS(2000));
    
    if(res){
    #ifdef PLOT
      Serial.printf("MsgQ: Received y_actual: %f y_pred: %f\n", rxY_vals.y_actual, rxY_vals.y_pred);
    #endif
        }
        else
          Serial.printf("MsgQ: Failed to receive data\n");
    #ifdef PLOT
        plot(rxY_vals.y_actual, rxY_vals.y_pred,rxY_vals.flag); //calling plot function
    #endif
  } // end of while(1)
} // end of plotTask()



void plot(float y_actual, float y_pred,int fl){
    Serial.println("Comp_Actual Comp_Predicted Zero"); // To print the labels of the values being plotted.  
    Serial.print(y_actual);  // Plot the actual value
    Serial.print("\t");
    Serial.print(y_pred);     // Plot the predicted value
    Serial.print("\t");  
    Serial.print(0.0); 
    Serial.print("\t");  
      
 
} // end of plot()

void fplotTask(void* parameter) {
  Y_VALUES rxY_vals;
  bool res;
    #ifndef PLOT
       Serial.print("Entering plotTask. It is running on the core ");
       Serial.println(xPortGetCoreID());
       Serial.printf("plotTask: Param received is %d\n", *(int*)parameter);
    #endif
 // while(1)
 {

    #ifndef PLOT    
    delay(1000);
    Serial.print("plotTask running on core: ");
    Serial.println(xPortGetCoreID());
    #endif
    res = xQueueReceive(xQHandle, &rxY_vals, pdMS_TO_TICKS(2000));
    
    if(res){
    #ifdef PLOT
      Serial.printf("MsgQ: Received y_actual: %f y_pred: %f\n", rxY_vals.y_actual, rxY_vals.y_pred);
    #endif
       }
        else
          Serial.printf("MsgQ: Failed to receive data\n");
    #ifndef PLOT
        fplot(rxY_vals.y_actual, rxY_vals.y_pred,rxY_vals.flag); //calling plot function
    #endif
  } // end of while(1)
} // end of plotTask()

void fplot(float y_actual, float y_pred,int fl){
    Serial.println("Sin_Actual Sin_Predicted "); // To print the labels of the values being plotted.  
    Serial.print(y_actual);  // Plot the actual value
    Serial.print("\t");
    Serial.print(y_pred);     // Plot the predicted value
    Serial.print("\t");  
   
} // end of plot()


// Inference related variables
float s_x_val;
Y_VALUES s_y_vals;

void S_inferenceEngine(void) {
  float position = static_cast<float>(inference_count) /
                     static_cast<float>(kInferencesPerCycle);
    s_x_val = position * kXrange;
    s_y_vals.y_actual = sin(s_x_val);
    s_y_vals.y_pred   = s_predict(s_x_val);
    s_y_vals.y_actual *= 3;
    s_y_vals.y_pred *= 3;
    s_y_vals.flag=0;
   
    bool res =  xQueueSend(xQHandle, &s_y_vals, pdMS_TO_TICKS(2000));
    if (!res) {
      printf("MsgQ: Failed to send an item.\n");
    }
    inference_count += 1;
    if (inference_count >= kInferencesPerCycle) 
      inference_count = 0;
      
#ifndef PLOT
    delay(1000); // 1 second of delay between two predictions to be seen on the Serial monitor
    Serial.print("s_inferenceTask(): Executing on core ");
    Serial.println(xPortGetCoreID());
#endif
fplotTask(a);
}
