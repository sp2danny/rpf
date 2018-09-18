
/*
struct Operator
{

	virtual char MyChar() = 0;
	virtual void Create(std::string) = 0;

	static void Register(char, OperatorMaker);
	static clone_ptr<Operator> DispatchCreate(std::string);

	virtual void MatchDir   ( File&, FileMatchStack ) = 0;
	virtual void MatchFile  ( File&, FileMatchStack ) = 0;
	virtual void MatchLines ( File&, LineMatchStack ) = 0;

private:
	static std::map<char, OperatorMaker> createMap;

};
*/

struct FileOperator final : Operator
{
	virtual char MyChar     (                       ) override;
	virtual void Create     ( std::string           ) override;
	virtual void MatchDir   ( File&, FileMatchStack ) override;
	virtual void MatchFile  ( File&, FileMatchStack ) override;
	virtual void MatchLines ( File&, LineMatchStack ) override;
};

