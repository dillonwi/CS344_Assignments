# NOTE to grader: Please run this script with Python3. Thank you! -Jack

import random
import os

# Create a file, assign it a unique name, and return the file
def createFile(numFiles, processID):
    name = str(numFiles) + "-" + str(processID)
    return open(name, "w+")

def randomChar():
    num = random.randint(0,26) + 97
    return str(chr(num))


# Create n files and populate them with 10 random characters in the lowercase
# alphabet.
def randomChars(n):
    numFiles = 0
    processID =  os.getpid()
    # Iterate n times
    for i in range(n):

        # Call createFile()
        file = createFile(numFiles, processID)
        numFiles = numFiles + 1

        # Iterate and call randomChar() 10 times. Print each character to stdout
        # and populate the file
        for j in range(10):
            c = randomChar()
            print(c, end='')
            file.write(c)
        print() # New line

        # Add an endline character at the end of the file, and print a newline char
        # to stdout
        file.write("\n")

        # Close the file
        file.close()

# Calculate n random numbers from 1 to 42 inclusive, print the numbers
# to stdout, and then multiply them.
def randomNums(n):
    numbers = list()
    product = 1

    for i in range(n):
        num = random.randint(1,42)
        numbers.append(num)
        product = product * num
        print(str(num))

    print(str(product))

# Main function
def main():
    randomChars(3)
    randomNums(2)

main()
