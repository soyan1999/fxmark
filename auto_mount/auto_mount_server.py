#!/usr/bin/env python3
import socket
import json
import subprocess
import sys

class AutoMountServer:
    def __init__(self, cfg):
        try:
            self.port = cfg['port']
            self.devs = cfg['devs']
            self.check_devs()
            self.mount_infs = [[dev, "", False] for dev in self.devs]
            self.dev_null = open("/dev/null", "a")
            self.server = None
            self.client = None
        except:
            print('config wrong!')
            sys.exit(-1)

    def exec_cmd(self, cmd):
        p = subprocess.Popen(cmd, shell=True, stdout=self.dev_null, stderr=self.dev_null)
        p.wait()
        return p.returncode

    def check_devs(self):
        pass

    def umount(self, mount_point):
        for mount_inf in self.mount_infs:
            if mount_inf[1] == mount_point and mount_inf[2] == True:
                self.exec_cmd("umount %s" % (mount_point,))
                print("umount %s" % (mount_point,))
                mount_inf[2] = False
                return True
        return False

    def umount_all(self):
        for _, mount_point, is_mounted in self.mount_infs:
            if is_mounted:
                self.exec_cmd("umount %s" % (mount_point,))

    def mount(self, mount_point):
        for mount_inf in self.mount_infs:
            if mount_inf[2] == False:
                dev = mount_inf[0]
                return_code = self.exec_cmd("mkfs.ext4 -F %s" % (dev,))
                if return_code != 0:
                    return False
                return_code = self.exec_cmd("mount -t ext4 %s %s" % (dev, mount_point))
                if return_code != 0:
                    return False
                return_code = self.exec_cmd("chmod 777 %s" % (mount_point,))
                if return_code != 0:
                    return False
                mount_inf[1] = mount_point
                mount_inf[2] = True
                print("mount -t ext4 %s %s" % (dev, mount_point))
                return True
        return False

    def success(self):
        self.client.send('success'.encode('utf-8'))
        self.client.close()

    def fail(self):
        self.client.send('fail'.encode('utf-8'))
        self.client.close()

    def run(self):
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.bind(('127.0.0.1', self.port))
        self.server.listen(12)
        while True:
            self.client, _ = self.server.accept()
            cmd = self.client.recv(4096).decode('utf-8')
            cmds = cmd.split()
            if cmds[0] == 'mount':
                if len(cmds) == 2 and self.mount(cmds[1]):
                    self.success()
                else:
                    self.fail()
            elif cmds[0] == 'umount':
                if len(cmds) == 2 and self.umount(cmds[1]):
                    self.success()
                else:
                    self.fail()
            elif cmds[0] == 'stop':
                self.umount_all()
                self.success()
                break
        self.server.close()

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('nead one config path')
    else:
        with open(sys.argv[1], 'r') as f:
            cfg = json.load(f)
            server = AutoMountServer(cfg)
            server.run()
