#!/usr/bin/python
import subprocess as sub
import threading
import datetime
import sys
import os
import getpass

TotalScore = 0.0;
CaseDir = '/home/routing_test/TotalCase/'

class RunCmd(threading.Thread):
    def __init__(self, cmd, polyfile, timeout):
        threading.Thread.__init__(self)
        self.cmd = cmd
        self.timeout = timeout
        self.polyfile= polyfile

    def run(self):
        #global TotalScore
        self.p = sub.Popen(self.cmd, stdout=sub.PIPE, shell=True)
        #out, err = self.p.communicate()
        #for line in out.splitlines():
        #    print line
        #    lines = line.split(":")
        #    if len(lines) == 2 and lines[0] == "Total Score":
        #        TotalScore += int(lines[1])
        self.p.wait()

    def Run(self):
        starttime = datetime.datetime.now()
        self.start()
        self.join(self.timeout)

        if self.is_alive():
            print "Time Out!\n"
            self.p.terminate()      #use self.p.kill() if process needs a kill -9
            self.join()
        else:
            global TotalScore
            endtime = datetime.datetime.now()
            print "Runing user exe..."
            seconds = (endtime - starttime).seconds;
            print "User exe time cost: ", seconds
            time_score = (seconds - 20) / 2.0 
            if time_score < 0.0:
                time_score = 0.0
            if time_score > 15:
                time_score = 15
            print "User exe time cost score: ", -1 * time_score

            id = self.polyfile.find('_')
            
            cmd = 'check ./centerline' + self.polyfile[id:]
            p = sub.Popen(cmd, stdout=sub.PIPE, shell=True)
            out, err = p.communicate()
            for line in out.splitlines():
                print line
            lines = line.split(":")
            if len(lines) == 2 and lines[0] == "Total Score":
                TotalScore += float(lines[1])
                TotalScore -= time_score
            
def main():
    if len(sys.argv) != 2:
        print "Usage: run.py xxFolder/lineExt"
        return
    exe = sys.argv[1]
    os.environ['CASE_DIR'] = CaseDir
    fileList = os.listdir(CaseDir)
    for f in fileList:
        if f.startswith('boundary'):
            cmd = exe + ' ' + CaseDir +  f
            print '\n\n==================== ', cmd, ' ===================='
            RunCmd(cmd,  f , 60).Run()

    print "\n========================="
    print "Summay:"
    print "User:", getpass.getuser()
    print "FINAL SCORE:", TotalScore
    
    logName = "/var/tmp/score/"
    log = open(logName + getpass.getuser() + ".txt", 'w')
    log.write(str(TotalScore))
    log.close()

if __name__=="__main__":
    main()
