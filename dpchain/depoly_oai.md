### 启动 OAI 栈（在 /ci-scripts/yaml_files/5g_rfsimulator）：

```
 sudo docker compose up -d mysql oai-amf oai-smf oai-upf oai-ext-dn
 sudo docker compose up -d oai-gnb oai-nr-ue
 进入容器
 
 sudo docker exec --privileged -it rfsim5g-oai-nr-ue /bin/bash
 sudo docker exec --privileged -it rfsim5g-oai-gnb /bin/bash
 
 确认 IP：
 ip addr
```

### 编辑 p3chain config

1. 编辑配置文件

// TODO，地址需要指向 OAI 监听的那个端口，现在是 docker 的端口，没走 OAI

`OAIbasedP3Chain/dpchain/conf/config.ini`

```
[BooterBasic]
NewAddressMode = false
BooterKeyAddress = ""
KeyStoreDir = "./booterKeyStore"
ServerName = "booter"
ListenAddress = "192.168.71.140:2013" // 这个
InitBooterAddress = "192.168.71.140:30300" // 这个
NATKind = "Any"
BootstrapNodesFilePath = "./booters/booter.txt"
MaxPeerNum = 10
CentralConfigMode = true
IsDisturbBooterURL = true

[DperBasic]
NewAddressMode =  false
DperKeyAddress = ""
DperPrivateKey = ""
AccountsDir =  "./accounts"
ServerName = "dper"
ListenAddress = "12.1.1.6:20131" //这个
InitBooterAddress = "192.168.71.140:30300" //这个
NATKind = "Any"
BootstrapNodesFilePath = "./booters/booter.txt"
MaxPeerNum = 10
MemoryDBMode = false
BlockDBPath = "./database/blockchain"
StorageDBPath = "./database/storage"
DperRole = "Follower"
SubNetName = "original"
CentralConfigMode = true
ContractEngine = "DEMO_CONTRACT_MIX123"
```

2. 生成 auto 目录

// TODO 启动脚本后续可以优化，这部分代码重新写一个更符合我们配置要求的

`go run initConfig.go`

3. 启动 booter 和 dper，log 可以反馈是不是连接上了

```
sudo docker exec -it rfsim5g-oai-gnb /bin/bash
cd /dpchain/auto/dper_booter1
./start.sh  
```

```
sudo docker exec -it rfsim5g-oai-nr-ue /bin/bash
cd /dpchain/auto/dper_dper1     # 其他节点 dper_dper2/3...
./start.sh  
```