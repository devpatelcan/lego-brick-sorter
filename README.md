# lego-brick-sorter


<a href="https://www.youtube.com/watch?v=SKIDw2HNKAg">
  <img src="https://img.youtube.com/vi/SKIDw2HNKAg/maxresdefault.jpg" width="600">
</a>


[Click here or on the image to watch the video](https://www.youtube.com/watch?v=SKIDw2HNKAg)

This project is an autonomous sorting system that uses a ResNet18 deep learning model to classify images of Lego bricks and sort them based on various physical attributes. Unlike standard sorters, this system features a real-time image classification convoloutional neural network (CNN), allowing the user to swap sorting logic (Color, Stud Count, Length, or Shape) dynamically via software presets.

## Core Capabilities

* **Sub-800ms Latency**: Completes a full cycle—image capture, LAN transmission, ResNet18 inference, decision-making, and image archival—in under **800 ms**.
* **91% Inference Accuracy**: Achieves high-reliability classification of **15 unique brick types** using a fine-tuned **ResNet18** architecture.
* **Servo Diverting Gate**: Features a single servo-controlled deflection guide. By leveraging the belt's existing kinetic energy, the gate steers bricks into specific bins without the need for multiple actuators.
* **Dual-Belt Isolation System**: Utilizes a secondary "high-speed" conveyor belt and a physical slide to singulate bricks, ensuring only one object enters the camera frame at a time.
* **Automated Operation**: Synchronizes precisely calculated motor **RPMs** and sensor reaction timing to maintain consistent spacing and system flow.

