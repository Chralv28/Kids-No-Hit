// Call the checkbox and display elements (switch and color)
const toggleSwitch = document.getElementById('toggleSwitch');
const statusDisplay = document.getElementById('status');

// Function to update localStorage and redirect
function updateSwitchState() {
    // Store the state of the toggle in localStorage
    localStorage.setItem('switchState', toggleSwitch.checked);

    // Redirect to the appropriate page based on the switch state
    if (toggleSwitch.checked) {
        window.location.href = '/here.html';
        statusDisplay.textContent = 'Bus is here!';
    } else {
        window.location.href = '/index.html';
        statusDisplay.textContent = 'Bus is not here';
    }
}

// Add an event listener to update the status when the switch is toggled
toggleSwitch.addEventListener('change', updateSwitchState);

// On page load, check if there is a stored state for the switch
window.addEventListener('load', function() {
    const savedState = localStorage.getItem('switchState');

    if (savedState !== null) {
        // Set the switch state based on the stored value
        toggleSwitch.checked = (savedState === 'true');
    }
});