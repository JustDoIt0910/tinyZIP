# TinyCompressor

#### 使用huffman编码的简单压缩工具。建立huffman树后，转换成范式huffman编码进行保存。支持文件和文件夹的压缩。

##### 编译

```sh
git clone https://github.com/JustDoIt0910/TinyCompressor.git
cd TinyCompressor
mkdir build && cd build
cmake ..
make
```



#### usage

```sh
# 压缩
./compress -c <folder>
# 解压
./compress -d <zip file>
```



![](https://github.com/JustDoIt0910/MarkDownPictures/blob/main/TinyCompressor1.png)

![](https://github.com/JustDoIt0910/MarkDownPictures/blob/main/TinyCompressor2.png)



#### TODO

- [x] huffman压缩
- [x] 文件夹打包
- [ ] LZW压缩
- [ ] BWT+RLE