## tmux

> 主要来自：
>
> 1. [Tmux 速成教程：技巧和调整](http://blog.jobbole.com/87584/)
> 2. [用 tmux 保持远程 ssh 连接](https://bitmingw.com/2017/04/11/tmux-keep-session-alive/)
>
> 注：下述`ctrl-b`均是指先按下再松开后，按后续指令

- 打开一个新的`tmux session`：终端输入`tmux`
- 左右分栏：`ctrl-b, %`
- 水平分栏：`ctrl-b "`

- 开启鼠标上下滚动： `ctrl-b, :`进入命令行，再输入`setw mouse on`（此时可以利用shift再点鼠标来进行其他操作）

### 远程相关

- `tmux ls`：用于查看当前所有已创建的 tmux 会话
- `tmux new -s name` ：创建一个名为 "name" 的新会话。tmux 默认的会话名称是 0、1、2... 为了便于记忆，最好在会话创建时指定一个名称。会话的名称可用 `tmux ls` 查看
- `tmux detach` ：卸载当前会话。为了保证这个指令可以被 shell 读取，应使用 `&` 字符让任务在后台运行。`tmux detach` 并没有切断 ssh 连接，它只是将当前 shell 交付给 tmux 管理而已。尽管快捷键 `Ctrl-b d` 可以起到相同的效果，但由于一些 ssh 终端不支持发送 `Ctrl-b`，`tmux detach` 在实用性上更胜一筹。
- `tmux rename -t old-name new-name` ：可以修改一个会话的名称。
- `tmux attach -t name` ：装载一个已创建的会话。通常在下次 ssh 登录时，使用它恢复先前的状态。
- `tmux kill-session -t name` ：销毁一个会话。在 tmux 看来这等同于断开 ssh 连接，也就是说，正在运行的任务会因 SIGHUP 而退出。

> 快捷的方式：采用  ctrl-b + d就默认放到后台运行
>
> 要查看的话：tmux attach-session-t id

默认在后端运行，更方便的方式可以采用：

```shell
nohup some_command > nohup2.out&  # 指定输出文件, 默认为nohup.out
```

