from flask import Flask, render_template, request, redirect, url_for, make_response
import subprocess
import logging
import json
import html



logging.basicConfig(filename='app.log', level=logging.INFO)


app = Flask(__name__)


@app.route('/error')
def display_error():
    error_message_1 = request.args.get('error_1')
    error_message_2 = request.args.get('error_2')
    return render_template('error.html', error_message_1=error_message_1,error_message_2=error_message_2)

def validate_memoryFormat():
    logging.info('entered')
    memory = {}
    with open("Memory.txt", 'r') as file:
        for line in file:
            line = line.strip()
            if line != '':
                try:
                    address, val = line.split() # first check for format
                    address = int(address)
                    val = int(val)
                    if address % 4 != 0:
                        logging.info('memory wrong')
                        raise ValueError("Memory address is not divisible by 4")
                except ValueError:
                    raise ValueError("Memory address is not divisible by 4")
            logging.info('memory correct')


def validate_assemblyCode():
    errorMsg = ''
    with open('errorMessages.txt', 'r') as f:
        text = f.read()
        errorMsg = text
    logging.info(f"error: {errorMsg}")
    return errorMsg


def display_file():
    with open('small_output.txt', 'r') as f:
        text = f.read()
    start_index = 0
    subtexts = []
    while True:
        start_index = text.find("PC is", start_index)
        if start_index == -1:
            break
        end_index = text.find("Register x31", start_index)
        line_end_index = text.find("\n", end_index)
        if line_end_index == -1:
            subtext = text[start_index:]
        else:
            subtext = text[start_index:line_end_index]
        subtexts.append(subtext)
        start_index = line_end_index
    return render_template('output_table.html', lines_list=subtexts, current_index = 0)

def display_file_simple():
    instructions = []
    register_values = []
    memory = []
    instToRegisters = {}
    inst = ''
    with open('output.txt', 'r') as f:
        for line in f:
            if line[0].isdigit():
                memory.append(line.strip())
            # elif line.startswith('Register x0'):
            #     register_values.clear()
            #     register_values.append(line.strip())
            elif line.startswith('Register'):
                register_values.append(line.strip())
                val = line.strip().split(':')[1]
                instToRegisters[inst].append(val)
            elif line.startswith('pc'):
                instructions.append(line[2:])
                inst = line[2:].split(':')[1].strip()
                instToRegisters[inst] = []
    return render_template('output_table_simple_2.html', Instructions = instructions, Registers = register_values, Memory = memory, instToRegistersJSON = instToRegisters)

@app.route('/')
def index():
    # each time the home page is visited or refreshed all these files get deleted.
    files = ['data.txt', 'Memory.txt', 'errorMessages.txt','assemblyCode.txt', 'output.txt']
    for file in files:
        with open(file, 'w') as f:
            f.truncate(0)
    return render_template('index.html')

@app.route('/submit', methods=['POST']) # to submit data to server
def submit():
    # form: firstAddress >> input, , mem_init >> c, mem_text, code_txt, format >> outputForm, mem_option
    fAddr = request.form['firstAddress']
    mem_init = request.form['mem_init']
    mem_option = request.form['mem_option']
    code_option = request.form['code_option']
    format = request.form['format']

    if mem_init == "yes":
        if mem_option == "file":
            uploaded_file = request.files['mem_file'] # 'file' should match the name attribute of the input element
            if uploaded_file.filename != '':
                uploaded_file.save(uploaded_file,"Memory.txt") # save the file to disk
        else:
            mem_value = request.form['mem_text']
            with open("Memory.txt", 'w') as ff:
                # it may require data processing
                ff.write(mem_value)
        try:
            validate_memoryFormat()
        except ValueError as e:
            logging.info("entered error handling for memory")
            return redirect(url_for('display_error', 
                            error_1 = "All memory addresses given should be aligned and divisible by 4",
                            error_2 = "Please correct the initial memory values and try again."))
        

    if code_option == "text":
        code_txt = request.form["code_txt"]
        with open("assemblyCode.txt", 'w') as fff:
            fff.write(code_txt)
    else:
        uploaded_file = request.files['code_file'] # 'code_file' should match the name attribute of the input element
        if uploaded_file.filename != '':
            uploaded_file.save("assemblyCode.txt") # save the file to disk

    with open('data.txt', 'w') as f:
        f.write(fAddr) # input
        f.write(" ")
        f.write(mem_init) # c
        f.write(" ")
        f.write(format) # outputForm

    # Run the compiled program and wait for it to finish
    process = subprocess.Popen('./a')
    logging.info('Running C++ program')
    process.wait()
    logging.info("c++ program ran successfully")
    # check for errors:
    if validate_assemblyCode() != '':
        logging.info('error in assembly code')
        return redirect(url_for('display_error', error_1 = validate_assemblyCode(), error_2 = ''))
    # # Redirect to output_table.html
    return redirect('/output')

@app.route('/output')
def output():
    # Display output
    #return display_file()
    return display_file_simple()

if __name__ == '__main__':
    app.run(debug=True)
