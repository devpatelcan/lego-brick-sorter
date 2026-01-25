# Autonomous Brick Sorter for Legos


<a href="https://www.youtube.com/watch?v=SKIDw2HNKAg">
  <img src="https://img.youtube.com/vi/SKIDw2HNKAg/maxresdefault.jpg" width="600">
</a>


[Click here or on the image to watch the video](https://www.youtube.com/watch?v=SKIDw2HNKAg)

This project is an autonomous sorting system that uses a ResNet18 deep learning model to classify images of Lego bricks and sort them based on various physical attributes. Unlike standard sorters, this system features a real-time image classification convolutional neural network (CNN), allowing the user to swap sorting logic (Color, Stud Count, Length, or Shape) dynamically via software presets.

## Core Capabilities

* **Sub-800ms Latency**: Completes a full cycle - image capture, LAN transmission, ResNet18 inference, decision-making, and image archival - in under **800 ms**.
* **91% Inference Accuracy**: Achieves high-reliability classification of **15 unique brick types** using a custom-made **ResNet18** architecture.
* **Servo Diverting Gate**: Features a single servo-controlled deflection guide. By leveraging the belt's existing kinetic energy, the gate steers bricks into specific bins without the need for multiple actuators.
* **Dual-Belt Isolation System**: Utilizes a secondary "high-speed" conveyor belt and a physical slide to singulate bricks, ensuring only one object enters the camera frame at a time.
* **Automated Operation**: Synchronizes precisely calculated motor **RPMs** and sensor reaction timing to maintain consistent spacing and system flow.


## Pictures
![20260104_165002](https://github.com/user-attachments/assets/fa4c606b-cbc1-4781-bf50-6ecf5c313e51)
*Full sorting system from entry conveyor belt, to end sorting system.*

<br>


![20260104_164957](https://github.com/user-attachments/assets/9bad4933-7b1c-4af9-a204-c22ea169d4ad)
*Fully sorted bricks using a "COLOUR" preset.*

<br>

![20260107_183455](https://github.com/user-attachments/assets/8b31e38e-ec61-4061-80b3-b9cc13a3dd95)
*Top-down view of the sorting system, including the centering tabs on conveyor belt 1 (the slower belt).*

<br>

![20260104_165021](https://github.com/user-attachments/assets/b14a4339-bf87-4be7-b4f0-483441ef8342)
*ResNet18 predictions in the terminal as well as archived brick images for future training to improve model accuracy.*

<br>

**Feel free to view the resources developed to get this brick sorter come to life in this repository!**

    



