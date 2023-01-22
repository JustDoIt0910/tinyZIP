

# TinyCompressor

#### 实现了huffman编码和LZW两种压缩算法的简单压缩工具。支持文件和文件夹的压缩。

#### 编译

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

##### 压缩文件格式

| 字段       | 长度 | 值     |
| ---------- | ---- | ------ |
| 压缩文件头 | 2    | 0x5A52 |

文件夹头部
| 字段         | 长度 | 值                 |
| ------------ | ---- | ------------------ |
| 文件夹标识   | 1    | 0x44               |
| 文件夹名长度 | 1    | n(0表示文件夹结束) |
| 文件夹名     | n    |                    |

文件头部
| 字段         | 长度 | 值   |
| ------------ | ---- | ---- |
| 文件标识     | 1    | 0x46 |
| 文件名长度   | 1    | n    |
| 文件名       | n    |      |
| 压缩文件数据 |      |      |

压缩数据格式(huffman)

| 字段                   | 长度  | 值                         |
| ---------------------- | ----- | -------------------------- |
| 压缩算法               | 1     | 0x48(huffman) 0x4E(无压缩) |
| 头部长度               | 2     | header len                 |
| 文件内容长度           | 4     | content len                |
| 1-16位码长symbol种数   | 16    | N1, N2, ... N16            |
| symbols                | ∑(Ni) | symbols                    |
| padding 长度(字节对齐) | 1     | padding len                |
| 压缩数据               |       |                            |

压缩数据格式(LZW)

| 字段     | 长度 | 值            |
| -------- | ---- | ------------- |
| 压缩算法 | 1    | 0x4C(LZW压缩) |
| 压缩数据 |      |               |

![](https://github.com/JustDoIt0910/MarkDownPictures/blob/main/TinyCompressorDemo1.png)

![](https://github.com/JustDoIt0910/MarkDownPictures/blob/main/TinyCompressorDemo2.png)



**主函数创建压缩器时可以指定不同的编解码器(huffman/lzw)**

```c
int main(int argc, char* argv[])
{
    ...
    comp_compressor_t* c = comp_compressor_init(COMP_CODEC_HUFFMAN);
     //comp_compressor_t* c = comp_compressor_init(COMP_CODEC_LZW);
    ...
}
```



实测huffman普适性较强，多数情况下压缩率为50%~80%，除非文件过小，否则基本不会发生膨胀情况。LZW对于文本文件有较高压缩率，对于重复序列极少的二进制文件会出现压缩率很大甚至膨胀情况。

#### TODO

- [x] huffman压缩
- [x] 文件夹打包
- [x] LZW压缩
- [ ] BWT+RLE
