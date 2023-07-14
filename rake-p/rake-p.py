import subprocess
import socket

def classifier(filename):
    #variables and data structures to store our rakefile data
    port=0
    hostdata = dict()
    actiondata=[]
    actionset_num=0
    action_num=0
    #reading from the rakefile and storing content in our data structures
    address = filename
    file = open(address, "r")
    for line in file:
        temp = line.split()
        if temp != [] and temp[0][0]!="#":
            if temp[0] == "PORT":
                port = int(temp[2])
            if temp[0] == "HOSTS":
                i=2
                while i < len(temp):
                    hostdata[i-1] = temp[i]
                    i+=1
            if temp[0][0:9] == "actionset":
                actionset_num+=1
                action_num=0
            
            elif temp[0]!="PORT" and temp[0]!="HOSTS" and temp[0][0:9]!="actionset":
                str=""
                type=""
                remote=False
                if temp[0]!="requires":
                    action_num+=1
                for i in range(0, len(temp)):
                    if temp[i][0:7] == "remote-":
                        remote=True
                        str+=temp[i][7:len(temp[i])]
                        if i<len(temp)-1:
                            str+=" "
                        type="command"
                    elif temp[0]=="requires":
                        if temp[i] != "requires":
                            str+=temp[i]
                            if i<len(temp)-1:
                                str+=" "
                            type="file"
                    else:
                        str+=temp[i]
                        if i<len(temp)-1:
                            str+=" "
                        type="command"
                actiondata.append([actionset_num, action_num, remote, type, str])
                remote=False
                
    #close the rakefile after storing the data
    file.close()
    
    return port, hostdata, actiondata
    
#send commands to the servers   
def send(hostname, port, command):
    if hostname=="localhost":
        #create a socket for the client
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        #try to connect to the server
        ipaddr = socket.gethostbyname(hostname)
        client.connect((ipaddr, port))
        
        #attempt to send the command to the server
        client.sendall(command.encode())
        
        #attempt to recieve result from the localhost server and print it
        while True:
            returner = client.recv(1024).decode()
            print(returner)
            if returner=="end":
                break
        
        #close the connection when everything is done
        client.close()
        return "Done!"
    
    #if the server is a remote server, will fetch result later and close connection later as well
    else:
        #create a socket for the client
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        #try to connect to the server
        ipaddr = socket.gethostbyname(hostname)
        client.connect((ipaddr, port))
        
        #attempt to send the command to the server
        client.sendall(command.encode())
        
        #return the socket to recieve outputs and to be closed later
        return client
    
#returns a quote from the servers 
def getcost(hostname, port):
    #create a socket for the client
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    #try to connect to the server
    ipaddr = socket.gethostbyname(hostname)
    client.connect((ipaddr, port))

    #attempt to fetch a quote from the server
    client.sendall("rnum".encode())

    #attempt to recieve the quote from the server
    quote = client.recv(1024).decode()
    
    #close the connection when everything is done

    client.close()
    
    return quote


def main():
    filename = "Rakefile.txt"
    port, hostdata, actiondata = classifier(filename)
    clients = []
    
    #sample printing tests
    #print(port)
    #print(hostdata)
    #print(actiondata)
    
    for item in actiondata:
        #if the command is local command (without remote- prefix) send the command to the localhost
        if item[2]==False:
            if item[3]=="command":
                res = send("localhost", port, item[4])
                print(res)
                
        elif item[2]==True:
            #attempt to get a quote from every server in order to send the command to the prioritized server to be executed
            costs = []
            for key in hostdata.keys():
                if hostdata[key]!="localhost":
                    cost = getcost(hostdata[key], port)
                    costs.append([cost, hostdata[key], port])
            #sort the costs list in order of the lowest qoute recieved from the server
            costs.sort(key=lambda x: x[0])
            print(costs)
            
            #sending the command to be executed to the server with lowest quote
            if item[3]=="command":
                client = send(costs[0][1], costs[0][2], item[4])
                clients.append(client)

#if all servers are busy
    #while(select.select(rlist, wlist, xlist[, timeout])):
        #recv()
            #close()
            #break            
    
main()
