#ifdef WORKBENCH

class OTT_FormatHelper
{
	static string FormatInteger(int number, string separator = "'")
	{
		string stringNumber = Math.AbsInt(number).ToString();
		string formattedInteger = "";
		
		int counter = 0;
		
		for (int i = stringNumber.Length() - 1; i >= 0; i--)
		{
			formattedInteger = stringNumber[i] + formattedInteger;
			counter++;
			
			if (counter % 3 == 0 && i != 0)
			{
				formattedInteger = separator + formattedInteger;
			}
		}
		
		if (number < 0)
		{
			formattedInteger = "-" + formattedInteger;
		}
		
		return formattedInteger;
	}
}