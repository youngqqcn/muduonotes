
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <google/protobuf/descriptor.h>
#include "test.pb.h"
#include <assert.h>

using namespace google::protobuf;

Message* createMessage(const std::string& );

int main(int argc, char const *argv[])
{
    Example1 exp1;
    exp1.set_stringval("hello world");
    exp1.set_bytesval("hello world");
    exp1.add_repeatedint32val(99);
    exp1.add_repeatedint32val(33);
    exp1.add_repeatedstringval("this is repeated1");
    exp1.add_repeatedstringval("this is repeated2");

    Example1_EmbeddedMessage *pEmbMsg = new Example1_EmbeddedMessage();
    pEmbMsg->set_int32val(99);
    pEmbMsg->set_stringval("this is embedded info");
    exp1.set_allocated_embeddedexample1(pEmbMsg);

    std::string filename = "output.data";
    std::fstream output(filename, std::ios::out | std::ios::binary | std::ios::trunc);

    std::string outStr;
    if (!exp1.SerializeToString(&outStr))
    {
        std::cout << "SerializeToString error:" << std::endl;
        return 1;
    }
    std::cout << outStr << std::endl;

    if (!exp1.SerializeToOstream(&output))
    {
        std::cerr << "Failed to serialize to file failed" << std::endl;
        return 1;
    }

    Example1 exp2;
    if (!exp2.ParseFromString(outStr))
    {
        std::cout << "ParseFromString error" << std::endl;
        return 1;
    }
    std::cout << "parsed successed" << std::endl;

    std::cout << "stringval:" << exp2.stringval() << std::endl;
    std::cout << "bytesVal:" << exp2.bytesval() << std::endl;
    std::cout << "embedded.int32val:" << exp2.embeddedexample1().int32val() << std::endl;
    std::cout << "embedded.stringval:" << exp2.embeddedexample1().stringval() << std::endl;

    for (int i = 0; i < exp2.repeatedint32val_size(); i++)
    {
        std::cout << "repeatedint32val[" << i << "]" << exp2.repeatedint32val(i) << std::endl;
    }

    for (int i = 0; i < exp2.repeatedstringval_size(); i++)
    {
        std::cout << "repeatedint32val[" << i << "]" << exp2.repeatedstringval(i) << std::endl;
    }


    std::string typeName = Example1::descriptor()->full_name() ;
    std::cout << typeName << std::endl;

    auto des = DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
    assert(des == Example1::descriptor());

    auto prot = MessageFactory::generated_factory()->GetPrototype(des);
    assert(prot == &Example1::default_instance());


    
    std::cout << "=========================================" << std::endl;

    auto *obj  = dynamic_cast<Example1*>(prot->New());
    if(!obj->ParseFromString(outStr))
    {
        std::cout << "parse error" << std::endl;
        return 1;
    }

    std::cout << "stringval:" << obj->stringval() << std::endl;
    std::cout << "bytesVal:" << obj->bytesval() << std::endl;
    std::cout << "embedded.int32val:" << obj->embeddedexample1().int32val() << std::endl;
    std::cout << "embedded.stringval:" << obj->embeddedexample1().stringval() << std::endl;

    for (int i = 0; i < obj->repeatedint32val_size(); i++)
    {
        std::cout << "repeatedint32val[" << i << "]" << obj->repeatedint32val(i) << std::endl;
    }

    for (int i = 0; i < obj->repeatedstringval_size(); i++)
    {
        std::cout << "repeatedint32val[" << i << "]" << obj->repeatedstringval(i) << std::endl;
    }

    //============================

    Message* pMsg = createMessage("Example1");
    if(nullptr == pMsg)
    {
        std::cout << "createMessage error" << std::endl;
        return 1;
    }
    if(!pMsg->ParseFromString(outStr))
    {
        std::cout << "parse error" << std::endl;
        return 1;
    }
    assert( typeid(*pMsg) == typeid(Example1::default_instance()) );

    return 0;
}


Message* createMessage(const std::string& typeName)
{
    Message* pMsg = nullptr;
    const Descriptor* pDescptor = DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
    if(pDescptor)
    {
        const Message* prototype = MessageFactory::generated_factory()->GetPrototype(pDescptor);
        if(prototype)
        {
            pMsg = prototype->New();
        }
    }
    return pMsg;
}