import re

#check if a string is valid according to a regex
def valid_regex(regex,string):
    extracted_regex = regex.findall(string)
    found = False
    if(len(extracted_regex) != 0):
        found = True
    return found,extracted_regex
token_types = {
    'SEMICOLON':';',
    'IF':'if',
    'THEN':'then',
    'END':'end',
    'REPEAT':'repeat',
    'UNTIL':'until',
    'READ':'read',
    'WRITE':'write',
    'ASSIGN':':=',
    'LESSTHAN':'<',
    'EQUAL':'=',
    'PLUS':'+',
    'MINUS':'-',
    'MULT':'*',
    'DIV':'/',
    'OPENBRACKET':'(',
    'CLOSEDBRACKET':')',
    'IDENTIFIER':None,
    'NUMBER':None
}

identifier_regex = re.compile('[a-z][a-z]*[0-9]*')
number_regex = re.compile('[0-9][0-9]*')

test = "thomas := x*x - y*y;"
splitted = test.split()

for s in splitted:
    found, extracted_regex = valid_regex(identifier_regex, s)
    if (found == True):
        print(extracted_regex)