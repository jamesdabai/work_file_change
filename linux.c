{FH8632
	{编译命令2019-1-20
		编译demo直接用scons命令即可：如果提示找不到环境变量则“重新运行下/pc/install_xxx.sh”文件，
		会重新安装编译环境即可。
		编译uboot：make  fh8632
		export PATH=/opt/xxx/xx/xx/bin/:$PATH  ：设置全局变量路径
		touch:创建文件
		mkdir:创建文件夹
		rz :win到linux传输文件
		sz :linux到win传输文件
	}
	{map文件2019-1-21
		code，rodata,data，bss:分别是，代码段，只读数据，已初始化全局变量，未初始化全局变量。
		flash空间=code+rodata+data;
	}
	{FH8632启动流程2019-1-21
		资源：
			4M flash ：存放代码镜像，类似磁盘；
			64M DDR  : 运行代码的地方，用于代码运行和全局变量；
			128K SRAM: 静态随机存储器，速度比DDR快，用于栈分配->貌似没有
		启动模块：芯片固件->ramboot->u_boot->rt_thread;
								   ->rt_thread;
		芯片固件：用于启动和烧录ramboot以及u_boot的固件，不可更改；
		ramboot:  判断有U_boot的情况下跳转到u_boot,无u_boot的情况下直接运行rt_thread;
				  会将flash的代码搬运到DDR中运行;
		u_boot :  可以升级RT_THread,一些命令行解析，搬运RT_thread镜像到DDR;
		rt_thread:运行这个时u_boot就没用了，所以DDR空间有重合，rt_thread:a0000000-a1800000,
				  u_boot:a0800000-.......
		函数流程:
				  [u_boot]start.s(fh8632)->start_armboot(board.c)->main_loop()->
				  sf probe 0;sf read a0000000 80000 180000;go a0000000->[core.bin]->
				  start_gcc.S(startup/gcc)->rtthread_startup(汇编直接调用)->系统板级初始化
				  ->系统任务初始化->app任务初始化->开启任务调度，交由rt_thread os;
		main_loop:
				命令行实现:搜索U_BOOT_CMD就可以得到所有的u_boot命令行，可以配置宏;
				cmd命令打开配置宏：config_cmd_default.h头文件中;
				获取命令方法：已将U_BOOT_CMD定义的所有内容都放在了连续的地址中，用起始地址和
							结束地址标记，就可以链表查询了，在.lds文件中定义；
				print命令：打印环境变量
		flash空间分布：board.c文件中。
		USB驱动:修改xgd的驱动中.inf文件中的PID和VID值，修改为不能识别时，右键属性信息中的硬件ID
			
	}
	{FH8632疑问2019-1-21
		将flash代码加载到DDR中的实现代码在哪里？->被隐藏
		如何设置要编译那个文件，不编译那个文件？
		栈是用的DDR还是SRAM？->用DDR
		flash读写有问题：读出来的数据有部分错误-2019-2-19？-和flash的起始地址相关，
			update.h中地址定义有偏差，不知是flash的读写接口有问题，还是flash的对齐有问题。
	}
	{烧写固件方法2019-1-23
		串口烧录：Ymodem协议
				loady a0000000 core.bin:将core.bin文件加载到DDR的a0000000地址处
				sf probe 0:初始化flash
				sf earse 0x80000 0x200000 擦写flash偏移量和长度
				sf write a0000000 0x80000 0x200000 将DDR中的数据写到flash中
		网络烧录：Xmodem协议
				tftp a0000000 core.bin:将core.bin文件加载到DDR的a0000000地址处
				sf probe 0:初始化flash
				sf earse 0x80000 0x200000 擦写flash偏移量和长度
				sf write a0000000 0x80000 0x200000 将DDR中的数据写到flash中 
		{烧写u_boot
			ymodem协议：
				loady->选取flash.img文件->下载到DDR完成->sf probe 0->sf earse flashaddr len
				->sf write ddr_addr flash_offset len->reset即可
			下载：
				u_boot.bin必须制作成.img文件，然后写到flash的0地址（USB下载），这样的img文件
				才能被芯片内部固件所识别，才能正常启动，制作成的.img文件前面一部分是DDR2的配置
				参数，便于固件初始化DDR。
			u_boot恢复：当u_boot的flash区域被破坏时，需要短接触点，使得芯片是串口启动然后将u_boot
			    的.img文件下载到flash中。----一般u_boot不做修改。
			ramboot.bin和u_bootspl.bin两个文件是没用的，不需要关注。
			下载art_rtt.img,直接下载.img文件，这是厂商提供的文件。
			其他的core.bin和recovery.bin都直接下载bin文件就ok，不需要img文件了。
		}
		
				
				
	
	}
	{多任务
		
	}
}
{Linux
	{压缩,解压2019-1-22

		压缩命令：tar,zip,unzip,rar,unrar;
		压缩软件：gzip,bzip2,compress  ：gzip比bzip2压缩率更大些;
		后缀    ：
				.tar->只是一个大包的命令，没有压缩的功能;
				.tar.gz->用tar工具大包并用gzip工具压缩;
				.tar.bz2->用tar工具打包并用bzip2工具压缩;
				.tar.Z->用tar工具打包并用compress工具压缩;
				.rar->rar工具
				.zip->zip工具
		选项：
			-c: 建立压缩档案 
			-x：解压 
			-t：查看内容 
			-r：向压缩归档文件末尾追加文件 
			-u：更新原压缩包中的文件 
			-v：显示所有过程 
			-f: 后跟文件名
			独立命令：
			-z：有gzip属性的 
			-j：有bz2属性的 
			-Z：有compress属性的 
			-O：将文件解开到标准输出 
	}
	{文本命令grep 2019-1-22
		
	}
	{linux中的一些软件
		samba：win和linux共享文件的软件  sudo apt-get install samba
		
	}
	{source命令
		source命令也称为“点命令”，也就是一个点符号（.）。
		source命令通常用于重新执行刚修改的初始化文件，使之立即生效，
		而不必注销并重新登录。
		
	}
}
{SCONS构建编译环境
	
	
}
{rt_thread
	{设备
		{定时器
			10ms秒中断一次，系统滴答，获取滴答的函数是tick_1 = rt_tick_get()；
		}
		
	}
	{宏定义的函数
		RTM_EXPORT：
		MSH_CMD_EXPORT
		INIT_COMPONENT_EXPORT
		INIT_PREV_EXPORT
		
	}
	{pthread
		pthread:POSIX threads的简称，是线程的POSIX标准；
		
	}
	
}