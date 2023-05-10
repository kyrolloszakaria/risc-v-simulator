from flask import Flask, render_template, request, redirect
import subprocess
import logging

# Configure logging
logging.basicConfig(filename='app.log', level=logging.INFO)



app = Flask(__name__)

def display_file():
    with open('small_output.txt', 'r') as f:
        lines = f.readlines()
    return render_template('output_table.html', lines=lines)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/submit', methods=['POST'])
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
    # process = subprocess.Popen('./a')
    logging.info('Running C++ program')
    # process.wait()
    logging.info("c++ program ran successfully")
    # # Redirect to output_table.html
    return redirect('/output')

@app.route('/output')
def output():
    # Display output
    return display_file()

if __name__ == '__main__':
    app.run(debug=True)
