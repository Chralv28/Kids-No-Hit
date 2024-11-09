//call the checkbox and display elements (switch and color)
const toggleSwitch = document.getElementById('toggleSwitch')
const statusDisplay = document.getElementById('status')

//Add an event listener to update the status when the switch is toggled
toggleSwitch.addEventListener('change',function() {
    if (toggleSwitch.checked) { 
        statusDisplay.textContent = 'Bus is here!';
    } else {
       statusDisplay.textContent = 'Bus is not here'
    }
}); 

