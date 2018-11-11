### 1. 更新时dl.google.com连不上问题

可以更改下hosts，可以在[chinaz.com](http://ping.chinaz.com/dl.google.com)上面查找可用的hosts。

1. 在搜索栏输入`dl.google.com`，比如找到了`203.208.40.69`
2. 然后`/etc/hosts`，加入`203.208.40.69  dl.google.com  `
3. 在运行`sudo pat-get update`即可

