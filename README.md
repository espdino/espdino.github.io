🐢 ESP Dino — 16×16 LED Matrix Game

A retro-style Dino game built for a 16×16 NeoPixel LED matrix driven by an ESP microcontroller — complete with obstacles, player animation, and high-score tracking.

Live demo (HTML5 / Emulator): Coming Soon!
Hardware build gallery: Images / video links if available

🧠 Overview

ESP Dino is a minimalist endless runner where your pixel dinosaur dodges obstacles on a 16×16 LED grid controlled by an ESP board. Inspired by classic LED games and the Chrome Dino, this project demonstrates embedded game logic, animation sequences, and real-time input handling on low-resolution hardware.

Perfect for makers, LED art lovers, and anyone curious about microcontroller games.

📦 Features

✔️ Run on 16×16 NeoPixel LED matrices

✔️ Jump, duck, and avoid random obstacles

✔️ Smooth sprite animation on limited hardware

✔️ Simple web interface

✔️ Scoring + difficulty progression

📁 Repository Structure

📦 espdino.github.io

├── index.html         ← Homepage

├── script.js          ← Web interface

├── styles.css         ← Web UI styling

├── firmware/          ← ESP microcontroller source

│   ├── main.ino

│   ├── sprites.h

│   ├── game_logic.cpp

│   └── ...

├── assets/            ← Images, schematics, diagrams

├── docs/              ← Design notes, circuits, charts

└── README.md

🚀 Getting Started

🛠 Requirements

Make sure you have:

ESP microcontroller (e.g., ESP32 or ESP8266)

16×16 NeoPixel LED matrix

Power supply compatible with the matrix

📥 Installation

Clone this repository

git clone https://github.com/espdino/espdino.github.io.git


Open the Arduino/PlatformIO project in your editor

Install dependencies (NeoPixel libraries, etc.)

Upload firmware to your ESP device

Add exact library names / versions here to avoid build issues later.
Bulldogjob

🎮 How to Play

Jump: Press Jump

Duck: Press Squat

Score increases as you survive longer.

⚙️ Configuration
Setting	Description

PIN_MATRIX	GPIO pin for NeoPixel data

MATRIX_WIDTH	16

MATRIX_HEIGHT	16

DELAY_STEP	Game speed tuning


🤝 Contributing

Contributions are welcome! Here’s how you can help:

Report bugs via Issues

Add new game features

Improve the emulator

Share gameplay recordings

Please follow the GitHub flow:

Fork the repo

Create a topic branch

Submit a Pull Request

📜 License

This project is licensed under AGPL-3.0 — see the LICENSE file for details.


Follow the project on GitHub: https://github.com/espdino/espdino.github.io

💡 Tips for a great README
