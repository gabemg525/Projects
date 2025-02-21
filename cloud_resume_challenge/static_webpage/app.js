// app.js

// Replace with your deployed Azure Function endpoint URL.
const functionUrl = "https://resume-challenge-functions.azurewebsites.net/api/VisitorCountFunction?";

async function updateVisitorCount() {
  try {
    const response = await fetch(functionUrl);
    if (!response.ok) {
      throw new Error(`HTTP error! Status: ${response.status}`);
    }
    const data = await response.json();
    // Assuming the response JSON is in the format:
    // { Message: "Visitor count updated successfully.", CurrentVisitorCount: <number> }
    document.getElementById("visitor-counter").textContent = data.CurrentVisitorCount;
  } catch (error) {
    console.error("Error fetching visitor count:", error);
    document.getElementById("visitor-counter").textContent = "Error";
  }
}

// Update the visitor count when the page loads.
window.addEventListener("load", updateVisitorCount);
