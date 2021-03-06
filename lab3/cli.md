show ip interfaces   验证访问列表

show access-lists [ACL_#_or_name]    显示ACL中的语句

show ip access-list [ACL_#_or_name]  显示IP ACL中的语句





8.RIP
    R(config)# router rip   配置IP RIP
    R(config-router)# network IP_network_#  指定哪些接口参与路由选择进程
    R(config-router)# version 1|2   只接受和发送RIP版本1或2（默认只产生RIPv1更新，而两个版本的更新都接收）
    R(config-router)# ip rip send|receive version 1 | version 2 | version 1 2   在特定接口上控制在发送或接收RIP更新时使用哪个版本的RIP
    R# show ip protocols    查看在路由器上已配置并运行的所有IP路由选择协议
    R# show ip route [name_of_the_IP_routing_protocol]  查看路由选择表
    R# debug ip rip 帮助进行故障排除

 11.OSPF
    R(config)# router ospf process_ID   配置OSPF
    R(config-router)# network IP_address wildcard_mask area area_#  指定接口进入OSPF区域
    R(config-if)# ip ospf cost cost_value   给接口指定成本（取值范围是1-65535）
    R(config-router)# neighbor IP_address_of_neighbor   手动配置邻居
    R(config-router)# neighbor IP_address_of_neighbor priority value    指定邻居的优先级（取值范围是0-255，255为DR，0为不可能成为DR）
    R(config-if)# ip ospf network non-broadcast 手工配置非广播网络类型
    R(config-router)# area area_id stub 允许创建存根区域
    R(config-router)# area area_id stub no-summary  允许创建完全存根区域
    R(config-router)# area area_id nssa 允许创建次存根区域
    R(config-router)# area area_id virtual-link router_id   连接不连续区域为骨干区域0
    R(config-router)# area area_id range network_address network_mask   在区域边界路由器上汇总在给定区域中的路由器使用的IP地址空间到其他的区域
    R(config-router)# summary-address network_address network_mask  在自治系统边界路由器上汇总外部路由
    R# show ip ospf 概述所有OSPF相关的信息
    R# show ip ospf process-id  与上相同，而不仅仅是指定的进程
    R# show ip ospf border-routers  显示在此自治系统中所有ABR和ASBR的路由器ID
    R# show ip ospf database    显示链路状态数据库
    R# show ip ospf interface   显示接口的OSPF参数和其他的指定到此接口上的OSPF信息
    R# show ip ospf neighbor [detail]   显示每个OSPF邻居和邻接状态
    R# debug ip ospf adj    报告路由器建立到其他路由器的邻接关系进程
    R# debug ip ospf events 报告路由器上的OSPF事件
    R# debug ip ospf packet 报告OSPF LSA分组的内容

12.ACL
   R(config)# access-list 1-99|1300-1999 permit|deny source_IP_address [wildcard_mask] [log]    创建标准编号IP ACL
    R(config-if)# ip access-group ACL_# in|out  在指定接口上启动ACL
    R(config-line)# access-class standard_ACL_# in|out  在指定若干线路上启动ACL以限制对路由器的Telnet访问
    R(config)# access-list 100-199|2000-2699 permit|deny IP_protocol source_address source_wildcard_mask [protocol_information] destination_address destination_wildcard_mask [protocol_information] [log]  创建扩展编号IP ACL
    R(config)# ip access-list standard ACL_name 创建标准命名ACL（从IOS11.2开始支持）
    R(config-std-acl)# permit|deny source_IP_address [wildcard_mask]    输入标准ACL命令
    R(config)# ip access-list extended ACL_name 创建扩展命名ACL（从IOS11.2开始支持）
    R(config-ext-acl)# permit|deny IP_protocol source_IP_address wildcard_mask [protocol_information] destination_IP_address wildcard_mask [protocol_information] [log] 输入扩展ACL命令
    R(config-if)# ip access_group ACL_name in|out   启动命名ACL
    R# show ip interfaces   验证访问列表
    R# show access-lists [ACL_#_or_name]    显示ACL中的语句
    R# show ip access-list [ACL_#_or_name]  显示IP ACL中的语句
    R# clear access-list counters [ACL_#_or_name]   对指定ACL中的匹配计数器清零