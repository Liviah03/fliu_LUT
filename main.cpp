#include <iostream>
#include <string>
#include <cstdlib> 

#include <lorina/aiger.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/klut.hpp>


#include <mockturtle/io/aiger_reader.hpp>  // 读取 AIG 文件
#include <mockturtle/io/write_verilog.hpp> // 写入 Verilog 文件

#include <mockturtle/algorithms/lut_mapping.hpp> 

#include <mockturtle/views/mapping_view.hpp> //允许在不修改原始网络的基础上附加映射信息


int main(int argc, char* argv[])
{
    
    #ifdef _WIN32
        system("chcp 65001 > nul");
    #endif

    // 创建一个 AIG (与非门图) 网络对象
    mockturtle::aig_network aig;

    // 定义要读取的文件名
    const std::string filename = "adder.aig";

    // 使用 lorina 库读取 AIG 文件，并将其内容加载到 aig 网络对象中
    // aiger_reader 是 mockturtle 提供的一个适配器，用于连接 lorina 和 mockturtle
    auto const result = lorina::read_aiger(filename, mockturtle::aiger_reader(aig));

    // 检查文件是否读取成功
    if (result != lorina::return_code::success)
    {
        std::cerr << "[错误] 读取 AIG 文件失败: " << filename << std::endl;
        return 1; // 如果失败，则退出程序
    }
    std::cout << "[信息] 成功读取 adder.aig 文件" << std::endl;


    // 1. 在原始的 AIG 网络之上，创建一个 "mapping_view" (映射视图)。
    //    这个视图会存储映射的结果，但不会修改原始的 AIG 网络。
    mockturtle::mapping_view<mockturtle::aig_network> mapped_aig{aig};

    // 2. 设置 LUT 映射算法的参数。
    mockturtle::lut_mapping_params ps;
    ps.cut_enumeration_ps.cut_size = 4; // 设置 LUT 的输入数量 (k) 为 4

    std::cout << "[信息] 正在执行 LUT 映射 (k=4)..." << std::endl;
    mockturtle::lut_mapping(mapped_aig, ps);

    // 4. 将带有映射信息的视图直接写入 Verilog 文件。
    //    write_verilog 函数能够识别 mapping_view，并自动输出一个由 LUT 组成的网表。
    std::cout << "[信息] 正在将 LUT 网络写入 adder_mapped.v 文件..." << std::endl;
    mockturtle::write_verilog(mapped_aig, "adder_mapped.v");

    std::cout << "[信息] 任务成功完成！" << std::endl;

    return 0; // 程序正常退出
}

