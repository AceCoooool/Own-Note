# sklearn常用操作

### 1. 处理丢失数据

> `from sklearn.preprocessing import Imputer`

[Imputer](http://scikit-learn.org/stable/modules/generated/sklearn.preprocessing.Imputer.html)

`Imputer(missing_values='NaN', strategy='mean', axis=0, verbose=0, copy=True)`

- `missing_values`：默认`NaN`，可以选择int类型，即指定什么视为缺失值
- `strategy`：采用什么策略，即用什么来填充缺失值（可以选---`mean`，`median`，`most_frequent`）
- `axis`：0代表列，1代表行（即按某个维度进行处理）

1. `fit(X[,y])`：对X进行估算
2. `transform(X)`：填充X中缺失的值
3. `fit_transform(X[,y])`：拟合数据，并进行转换

> fit_transform = fit + transform

### 2. 解析数据

> `from sklearn.preprocessing import LabelEncoder`

[LabelEncoder](http://scikit-learn.org/stable/modules/generated/sklearn.preprocessing.LabelEncoder.html)

`le=LebalEncoder()`

- `classes_`：每个类别对应的标签

1. `fit(y)`：按照标签进行encoder
2. `transform(y)`：将标签转化为encoding后的标签
3. `fit_transform(y)`：按标签进行encoder，并将标签转化为encoding后的标签
4. `inverse_transform(y)`：将encoding后的标签转换为原来的标签

> `from sklean.preprocessing import OneHotEncoder`

[OneHotEncoder](http://scikit-learn.org/stable/modules/generated/sklearn.preprocessing.OneHotEncoder.html)

