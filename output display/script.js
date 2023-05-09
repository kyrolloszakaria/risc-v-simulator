// get the table bodies for executing instructions and PC, and register values
var executingTableBody = $("#executing-table tbody");
var registerTableBody = $("#register-table tbody");

fetch('small_output.txt')
  .then(response => response.text())
  .then(inputText => {
    // split the input text into separate instructions
    var instructions = inputText.split("\n\n");

    // loop through each instruction
    for (var i = 0; i < instructions.length; i++) {
      // split the instruction into separate lines
      var lines = instructions[i].split("\n");

      // if the first line contains "PC is", then it's an executing instruction
      if (lines[0].startsWith("PC is")) {
        // get the PC value from the first line
        var pc = lines[0].split(" ")[2];

        // get the executing instruction from the second line
        var instruction = lines[1].split(" ")[2];

        // create a new row in the executing table and append it to the table body
        var row = $("<tr>");
        var instructionCell = $("<td>").text(instruction);
        var pcCell = $("<td>").text(pc);
        row.append(instructionCell);
        row.append(pcCell);
        executingTableBody.append(row);
      }
      // otherwise, it's a register value
      else {
        // loop through each line and create a new row in the register table for each register value
        for (var j = 1; j < lines.length - 1; j++) {
          var parts = lines[j].split(" : ");
          var register = parts[0];
          var value = parts[1];

          var row = $("<tr>");
          var registerCell = $("<td>").text(register);
          var valueCell = $("<td>").text(value);
          row.append(registerCell);
          row.append(valueCell);
          registerTableBody.append(row);
        }
      }
    }
  })
  .catch(error => console.error('Error:', error));
