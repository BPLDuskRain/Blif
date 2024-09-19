# Analyzer
```Analyzer```类为解析.blif的类
- 先使用```readBlif```方法读入需要转换的.blif文件
    - 对于转为.v文件
    - 使用```analyze```方法进行转换
    - 最后使用```writeV```方法进行写入即可
    + 对于转为中间形式
    + 使用```toMidForm```方法进行转换
    + 最后使用```writeMidForm```方法进行输出
# Analyser
Analyser类为v转.blif的类
- 先使用readV方法读入需要转换的.v文件
- 使用analyse方法进行转换
- 最后使用writeBlif方法进行写入即可
