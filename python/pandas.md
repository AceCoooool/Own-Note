# pandas常用操作

下面默认采用：

```python
import pandas as pd
dataset = pd.read_csv('xxx.csv')
```



读取`.csv`文件：`dataset=pd.read_csv('Data.csv')` --- 其中得到的`dataset`为`DataFrame`类型

`DataFrame.iloc[行号索引, 列号索引]` --- 通过行号索引和列好索引来选取`DataFrame的`一个子集

`DataFrame.values` --- 以`np.array`形式返回`DataFrame`里面的内容（不包括"标题"和"序列号"）