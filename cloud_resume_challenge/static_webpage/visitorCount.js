// Simple localStorage-based visitor count
document.addEventListener('DOMContentLoaded', () => {
  // Get existing visitor count from localStorage (if any)
  let visitorCount = parseInt(localStorage.getItem('visitorCount') || '0');

  // Increase by one every page load
  visitorCount++;

  // Save back to localStorage
  localStorage.setItem('visitorCount', visitorCount.toString());

  // Create a small div element to display the count in the corner
  const counterDiv = document.createElement('div');
  counterDiv.id = 'visitorCountDisplay';
  counterDiv.textContent = `Visits (this browser): ${visitorCount}`;

  // Basic inline styling for a corner overlay
  counterDiv.style.position = 'fixed';
  counterDiv.style.bottom = '10px';
  counterDiv.style.right = '10px';
  counterDiv.style.padding = '8px 12px';
  counterDiv.style.backgroundColor = 'rgba(0,0,0,0.7)';
  counterDiv.style.color = '#fff';
  counterDiv.style.borderRadius = '4px';
  counterDiv.style.fontSize = '14px';
  counterDiv.style.fontFamily = 'Arial, sans-serif';

  // Add it to the page body
  document.body.appendChild(counterDiv);
});

