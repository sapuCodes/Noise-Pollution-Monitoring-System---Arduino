#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "SHELBY_";
const char* password = "12345678";

WebServer server(80);  // Set up web server on port 80
String soundData = "Waiting for data...";

void setup() {
  Serial.begin(115200);       // Serial Monitor
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17 (connect Arduino Uno to these pins)

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP()); // Print ESP32 IP address

  // Serve the HTML content on the root ("/") route
  server.on("/", []() {
    String html = R"rawliteral(
    <!DOCTYPE html>
<html>
<head>
  <title>Noise Monitor</title>
  <meta http-equiv="refresh" content="1">
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      margin: 0;
      padding: 0;
      background: linear-gradient(to right, #e0f7fa, #fce4ec);
      color: #333;
      text-align: center;
    }

    header {
      background-color: #006064;
      color: white;
      padding: 20px 0;
      box-shadow: 0 2px 8px rgba(0, 0, 0, 0.2);
    }

    h1 {
      margin: 0;
      font-size: 2em;
    }

    main {
      padding: 30px 20px;
    }

    #soundVal {
      font-size: 2em;
      color: #d84315;
    }

    #chart-container {
      background: white;
      border-radius: 12px;
      box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
      width: 90%;
      max-width: 700px;
      margin: 30px auto;
      padding: 20px;
    }

    canvas {
      max-height: 300px;
    }

    p {
      font-size: 1.2em;
    }
  </style>
</head>
<body>
  <header>
    <h1>Real-Time Noise Monitor</h1>
  </header>
  <main>
    <p>Current Sound Level: <b id="soundVal">)rawliteral" + soundData + R"rawliteral(</b></p>
    <div id="chart-container">
      <canvas id="soundChart"></canvas>
    </div>
  </main>

  <script>
    let data = Array(20).fill(0);
    let labels = Array.from({ length: 20 }, (_, i) => i + 1);
    let ctx = document.getElementById('soundChart').getContext('2d');
    let chart = new Chart(ctx, {
      type: 'line',
      data: {
        labels: labels,
        datasets: [{
          label: 'Sound Level',
          data: data,
          borderColor: '#ff7043',
          backgroundColor: 'rgba(255,112,67,0.2)',
          borderWidth: 3,
          tension: 0.3,
          pointRadius: 3,
        }]
      },
      options: {
        responsive: true,
        plugins: {
          legend: {
            labels: {
              color: '#333'
            }
          }
        },
        scales: {
          x: {
            ticks: { color: '#555' }
          },
          y: {
            beginAtZero: true,
            max: 1024,
            ticks: { color: '#555' }
          }
        }
      }
    });

    setInterval(() => {
      fetch("/value")
        .then(res => res.text())
        .then(val => {
          document.getElementById("soundVal").innerText = val;
          data.push(parseInt(val));
          data.shift();
          chart.update();
        });
    }, 1000);
  </script>
</body>
</html>
    )rawliteral";

    server.send(200, "text/html", html);
  });

  // Endpoint for real-time sound data
  server.on("/value", []() {
    server.send(200, "text/plain", soundData);
  });

  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();

  // Read data from Arduino Uno
  if (Serial2.available()) {
    soundData = Serial2.readStringUntil('\n');
    soundData.trim(); // Clean up extra characters
    Serial.println("Received from Uno: " + soundData);
  }
}
