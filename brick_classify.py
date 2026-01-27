from flask import Flask, request
import torch
import torch.nn as nn
from torchvision import models, transforms
from PIL import Image
import os
import time
import logging

log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR) 

app = Flask(__name__)

SORTING_PRESET = "COLOUR" 

BRICK_ATTRIBUTES = {
    "orange_2x2_n": {"COLOUR": "ORANGE", "STUDS": 4, "LENGTH_STUDS": 2, "SLANTED": False},
    "blue_4x1_n":   {"COLOUR": "BLUE",   "STUDS": 4, "LENGTH_STUDS": 4, "SLANTED": False},
    "white_4x2_n":  {"COLOUR": "WHITE",  "STUDS": 8, "LENGTH_STUDS": 4, "SLANTED": False},
    "orange_1x1_s": {"COLOUR": "ORANGE", "STUDS": 1, "LENGTH_STUDS": 1, "SLANTED": True},
    "red_4x1_n":    {"COLOUR": "RED",    "STUDS": 4, "LENGTH_STUDS": 4, "SLANTED": False},
    "blue_4x2_n":   {"COLOUR": "BLUE",   "STUDS": 8, "LENGTH_STUDS": 4, "SLANTED": False},
    "orange_4x1_n": {"COLOUR": "ORANGE", "STUDS": 4, "LENGTH_STUDS": 4, "SLANTED": False},
    "white_2x2_n":  {"COLOUR": "WHITE",  "STUDS": 4, "LENGTH_STUDS": 2, "SLANTED": False},
    "red_4x2_n":    {"COLOUR": "RED",    "STUDS": 8, "LENGTH_STUDS": 4, "SLANTED": False},
    "blue_2x2_n":   {"COLOUR": "BLUE",   "STUDS": 4, "LENGTH_STUDS": 2, "SLANTED": False},
    "white_4x1_n":  {"COLOUR": "WHITE",  "STUDS": 4, "LENGTH_STUDS": 4, "SLANTED": False},
    "red_1x1_s":    {"COLOUR": "RED",    "STUDS": 1, "LENGTH_STUDS": 1, "SLANTED": True},
    "orange_4x2_n": {"COLOUR": "ORANGE", "STUDS": 8, "LENGTH_STUDS": 4, "SLANTED": False},
    "white_1x1_s":  {"COLOUR": "WHITE",  "STUDS": 1, "LENGTH_STUDS": 1, "SLANTED": True},
    "red_2x2_n":    {"COLOUR": "RED",    "STUDS": 4, "LENGTH_STUDS": 2, "SLANTED": False}
}

BIN_MAP_COLOUR = {"BLUE": 1, "ORANGE": 2, "WHITE": 3, "RED": 4}
BIN_MAP_STUDS = {1: 1, 4: 2, 8: 3} 
BIN_MAP_LENGTH = {1: 1, 2: 2, 4: 3} 
BIN_MAP_SLANTED = {True: 1, False: 2} 

DEFAULT_BIN = 1 

class LegoClassificationModel(nn.Module):
    def __init__(self, num_classes=15):
        super().__init__()
        self.backbone = models.resnet18()
        num_fts = self.backbone.fc.in_features
        self.backbone.fc = nn.Linear(num_fts, num_classes)

    def forward(self, x):
        return self.backbone(x)

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model = LegoClassificationModel(num_classes=15).to(device)
MODEL_PATH = "lego_sorter_final5.pth" 
DEBUG_DIR = "debug_pics"

if not os.path.exists(DEBUG_DIR): os.makedirs(DEBUG_DIR)

if os.path.exists(MODEL_PATH):
    model.load_state_dict(torch.load(MODEL_PATH, map_location=device))
    model.eval()
    print("AI Model Loaded.")

CLASS_NAMES = list(BRICK_ATTRIBUTES.keys())

preprocess = transforms.Compose([
    transforms.Resize((448, 448)),
    transforms.ToTensor(),
    transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
])

@app.route('/upload', methods=['POST'])
def upload():
    img_data = request.data
    timestamp = int(time.time())
    save_path = os.path.join(DEBUG_DIR, f"brick_{timestamp}.jpg")
    with open(save_path, "wb") as f:
        f.write(img_data)
    
    image = Image.open(save_path).convert("RGB")
    input_tensor = preprocess(image).unsqueeze(0).to(device)
    
    with torch.no_grad():
        output = model(input_tensor)
        probabilities = torch.nn.functional.softmax(output, dim=1)
        confidence, predicted_idx = torch.max(probabilities, 1)

    prediction_name = CLASS_NAMES[predicted_idx.item()]
    brick_data = BRICK_ATTRIBUTES.get(prediction_name)
    target_bin = DEFAULT_BIN

    if brick_data:
        attr_value = brick_data.get(SORTING_PRESET)
        
        if SORTING_PRESET == "COLOUR":
            target_bin = BIN_MAP_COLOUR.get(attr_value, DEFAULT_BIN)
        elif SORTING_PRESET == "STUDS":
            target_bin = BIN_MAP_STUDS.get(attr_value, DEFAULT_BIN)
        elif SORTING_PRESET == "LENGTH_STUDS":
            target_bin = BIN_MAP_LENGTH.get(attr_value, DEFAULT_BIN)
        elif SORTING_PRESET == "SLANTED":
            target_bin = BIN_MAP_SLANTED.get(attr_value, DEFAULT_BIN)
        
    print(f"Detected: {prediction_name} | {SORTING_PRESET}: {attr_value} | -> BIN {target_bin}")
    
    return str(target_bin), 200

if __name__ == '__main__':

    app.run(host='0.0.0.0', port=5000)
