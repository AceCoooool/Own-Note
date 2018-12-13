> 最简单易懂的教程：[Git实用指南](http://rogerdudler.github.io/git-guide/index.zh.html)或者[Git廖雪峰版本教程](http://www.liaoxuefeng.com/wiki/0013739516305929606dd18361248578c67b8067c8c017b000)(对！没错，就是那个写了python, java等教程的那位！)
>
> 教育优惠申请：[教育优惠申请网址](https://education.github.com/)：tip，在申请时理由写的可以多一丢丢，此外，别忘了将你的`github`账号和你的学校邮箱绑定在一起。（通过的邮件往往是会被当成垃圾邮件，一般一天之内就会通过了。）
>
> 私人仓库"共享"给你希望能看的人：进入私人仓库->settings->Collaborators->输入对方的昵称->add collaborator->右上角有个链接，发给对方就OK了。

## 1. 基本操作

#### 添加和提交

- `git add <filename>`（常用`git add *`）：把它们添加到暂存区
- `git commit -m "代码提交信息"`：实际提交改动

#### 推送改动

- `git push origin master`：可以把 *master* 换成你想要推送的任何分支

#### 更新与合并

- `git pull`：更新你的本地仓库至最新改动
- `git merge <branch>`：以在你的工作目录中 *获取（fetch）* 并 *合并（merge）* 远端的改动。要合并其他分支到你的当前分支（例如 master）



## 2. 多个github账号





## 3. 其他命令

- `git clean -xfd`：清楚因为编译等产生的"文件"---编译失败后可以采用这个来清理后再重新编译

