### 一、设置配置文件

配置文件路径位于`/conf/config.ini`。主要有以下几项需要用户手动配置：

```ini
[Network]
DperCount = 100    	// 总共需要部署的节点数
GroupCount = 10		// 总共需要部署的分区数
LeaderCount = 1		// 每个分区Leader节点的数量(设置为1即可)
FollowerCount = 9	// 每个分区Follower节点的数量

[BooterBasic]
ListenAddress = "10.129.68.44:20130"   // Booter节点共识通信端口，IP需要设置为本机IP
InitBooterAddress = "10.129.68.44:30300"  // Booter节点初始化组网使用的通信端口，IP需要设置为本机IP
MaxPeerNum = 100		// Booter节点需要连接的Dper节点数量(与Network.DperCount保持一直即可)

[DperBasic]
ListenAddress = "10.129.68.44:20131"  		// Dper节点共识通信端口
InitBooterAddress = "10.129.68.44:30300"	// Booter节点初始化组网使用的通信端口,Dper节点需要事先知道
MaxPeerNum = 100

[DperRunMode]
DaemonMode = false			// Booter节点以及各Dper节点的程序是否以守护进程方式运行
GnomeTerminalMode = true	// 如果不以守护进程方式运行, 是否以Gnome可视化窗口启动各节点程序

[NetworkInit]
WaittingViewNet = 10    // 控制初始化组网最大等待时间(若组网失败,可以适当调大该等待时长)
```

### 二、运行初始化脚本

用户需要在一台机器上运行初始化脚本`/conf/initConfig.go`，即使用命令`go run initConfig.go`。启动后将在在当前目录中生成一个`auto`文件夹，其中包括各节点的节点文件夹以及启动`sh`脚本。

需要注意：需要在多机部署环境中的**主机器**（假设多机环境共有三台机器，那么会存在一台主机器+两台从机器）上运行`initConfig.go`，目的是为了让`Booter`节点实现启动一次并将自身`url`分发给其他所有`Dper`节点。

### 三、配置多机环境

多机环境可以分为：一台主机器 + 若干台从机器。 

由于后续需要通过多机启动脚本在主机器上一键启动所有的节点，主机器需要使用`ssh`和`scp`命令远程控制其余从机器。为此，主机器需要能够实现`SSH免验证`登录其他主机。

1. 在所有的机器上安装`ssh`

```bash
## 安装ssh
sudo apt install openssh-server   ## 安装ssh-server
sudo apt install openssh-client   ## 安装ssh-client
```

2. 将主机器的公钥发送给所有从机器

```bash
## 1.查看本地系统的主目录下是否存在.ssh/id_rsa.pub这个密钥文件,如果有会存在 id_rsa和id_rsa.pub
$ ls .ssh/
## 2.如果没有秘钥文件，则需要自行生成(全程回车)
$ ssh-keygen -t ras
## 3.将主机器的公钥发送给其他从机器（如果是普通用户abc，则以下操作目录为/home/abc）：
$ scp .ssh/id_rsa.pub root@server_ip_addr:/root
```

3. 从机器将获取的主机器公钥保存至`.ssh/authorized_keys`文件中

```bash
## 1.如果.ssh/authorized_keys已经存在，则直接将公钥文件内容拷贝至其中
$ cat id_rsa.pub >> .ssh/authorized_keys
## 2.如果不存在，则需要新建
$ mv id_rsa.pub .ssh/authorized_keys
```

### 四、生成多机运行文件

在主机器上，填写多机部署配置文件`/mul_machine/mul.json`

```json
{
    "MachineCount": 3,           	// 机器数量
    "BooterIP": "10.129.68.44",  	// Booter节点所在的主机器的IP地址
    "Machines":[
        {
            "IP":"10.129.68.44",    // 机器1的IP
            "DirPath":"/home/oem/dpchain_auto_delpoy_tool/auto/10.129.68.44",   // 机器1所有节点文件所在路径
            "StartNode":1,			// 该机器需要运行的节点的起始编号
            "EndNode":4,			// 该机器需要运行的节点的终止编号
            "HasBooter":true,    	// 该机器是否允许Booter(是否是主机器)

            "NetID":"net0",			// 该节点运行的节点们的子网编号
            "LeaderCount":1,		// 该节点运行的节点们的子网内Leader数量
            "FollowerCount":3		// 该节点运行的节点们的子网内Follower数量
        },
        {
            "IP":"10.129.118.55",
            "DirPath":"/home/jackson/auto/10.129.118.55",
            "StartNode":5,
            "EndNode":8,
            "HasBooter":false,

            "NetID":"net1",
            "LeaderCount":1,
            "FollowerCount":3
        },
        {
            "IP":"10.129.121.202",
            "DirPath":"/home/zwd/auto/10.129.121.202",
            "StartNode":9,
            "EndNode":12,
            "HasBooter":false,

            "NetID":"net2",
            "LeaderCount":1,
            "FollowerCount":3
        }
    ]
}
```

接着在主机器上运行`mul.go`文件，可以在`auto`文件中生成属于各`IP`主机的节点文件夹。

### 五、运行多机部署脚本

该脚本位于`/mul_machine/mul.sh`。

需要用户手动修改下述内容：

```bash
## 机器1的用户名
mulMachineUser_1="oem"      
## 机器1的IP
mulMachineIP_1="10.129.68.44"  
## 机器1在主机器上要使用的节点文件夹，由mul.go生成的各IP文件夹在主机器上的路径
mulMachineSrc_1="/home/oem/github/MyDP_Chain/dper/client/auto/10.129.68.44"   
## 主机器需要将机器1的IP文件夹通过scp命令移动至机器1的目的路径(如果当前机器就是主机器,那么Dst与Src一致即可，不需要移动)
mulMachineDst_1="/home/oem/github/MyDP_Chain/dper/client/auto/10.129.68.44"	  

mulMachineUser_2="jackson"
mulMachineIP_2="10.129.118.55"
mulMachineSrc_2="/home/oem/github/MyDP_Chain/dper/client/auto/10.129.118.55"
mulMachineDst_2="/home/jackson/auto/10.129.118.55"

mulMachineUser_3="zwd"
mulMachineIP_3="10.129.121.202"
mulMachineSrc_3="/home/oem/github/MyDP_Chain/dper/client/auto/10.129.121.202"
mulMachineDst_3="/home/zwd/auto/10.129.121.202"
```

运行多机部署脚本`bash mul.sh`